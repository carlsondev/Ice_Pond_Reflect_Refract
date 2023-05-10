//
// Created by bcarl on 2/9/2023.
//

#include "ice_pond.h"

#include <cmath>
#include <cstdio>

using namespace glm;

static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  if (action != GLFW_REPEAT) {
    return;
  }

  auto scene = (Assignment2Scene *)glfwGetWindowUserPointer(window);
  float iceSpeed = 0.5f;
  switch (key) {
  case GLFW_KEY_UP:
    scene->shiftIcePosition(vec3(0, iceSpeed, 0));
    break;
  case GLFW_KEY_DOWN:
    scene->shiftIcePosition(vec3(0, -iceSpeed, 0));
    break;
  case GLFW_KEY_LEFT:
    scene->shiftIcePosition(vec3(-iceSpeed, 0, 0));
    break;
  case GLFW_KEY_RIGHT:
    scene->shiftIcePosition(vec3(iceSpeed, 0, 0));
    break;
  case GLFW_KEY_W:
    scene->shiftIcePosition(vec3(0, 0, -iceSpeed));
    break;
  case GLFW_KEY_S:
    scene->shiftIcePosition(vec3(0, 0, iceSpeed));
    break;
  default:
    break;
  }
}

static void resizeCallback(GLFWwindow *window, int width, int height) {
  auto scene = (Assignment2Scene *)glfwGetWindowUserPointer(window);
  scene->resize(width, height);
}

static void mousePositionCallback(GLFWwindow *window, double xpos,
                                  double ypos) {
  auto scene = (Assignment2Scene *)glfwGetWindowUserPointer(window);
  scene->handleCursorPositionEvent(vec2(xpos, ypos));
}

static void mouseScrollCallback(GLFWwindow *window, double xoffset,
                                double yoffset) {
  auto scene = (Assignment2Scene *)glfwGetWindowUserPointer(window);
  scene->camera->moveForward(yoffset);
}

Assignment2Scene::Assignment2Scene(GLFWwindow *window) {
  this->currentWindow = window;

  glfwSetWindowSizeCallback(window, resizeCallback);

  int width, height = 0;

  glfwGetWindowSize(this->currentWindow, &width, &height);
  this->windowDimensions = glm::vec2(width, height);

  glfwSetKeyCallback(this->currentWindow, keyCallback);

  glfwSetWindowUserPointer(this->currentWindow, this);

  glfwSetCursorPosCallback(window, mousePositionCallback);

  glfwSetScrollCallback(window, mouseScrollCallback);
}

void Assignment2Scene::initScene() {
  this->compileShaderPrograms();

  // glEnable(GL_DEPTH_TEST);
  //   glEnable(GL_BLEND);
  //   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  this->camera = new ArcballCam(2.0f, 100.0f);
  this->camera->setRadius(50);
  this->envMapFBOObject =
      new EnvironmentMapFBOObject(this->ice_center_loc, this->windowDimensions);

  this->ground_plane = new Plane(100, 100, 1, 1);
  this->ground_base = new Cube(100.0f); // Scale smaller vertically
  this->ice = new Cube(2.0f);

  // Add green color to ground plane
  // this->addColorToObject(this->ground_plane, {0.49, 0.78, 0.47});

  this->addColorToObject(
      this->ground_plane,
      this->fillRandVec3Vector(this->ground_plane->getNumVerts()));

  // Add brown color to ground base
  this->addColorToObject(this->ground_base,
                         {78.0 / 255.0, 53.0 / 255.0, 36.0 / 255.0});

  // Add blue color to ice
  this->addColorToObject(this->ice,
                         {102.0 / 255.0, 153.0 / 255.0, 204.0 / 255.0});
  this->basicShadingProgram.use();
  this->activeShaderProgram = &this->basicShadingProgram;
}

void Assignment2Scene::update(float t) {
  this->systemTime = t;
  this->computeActiveMatrices();
}

// START OF RENDERING FUNCTIONS
void Assignment2Scene::render_ground() {

  // Render ground plane
  this->model = mat4(1.0f);
  this->model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
  this->model = glm::scale(model, glm::vec3(1, 1, 1));

  this->passMatrices();

  MaterialProperties groundMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  ground_plane->render();

  // Render ground base
  this->model = mat4(1.0f);
  this->model = glm::translate(model, vec3(0.0f, -6.0f, 0.0f));
  this->model = glm::scale(model, glm::vec3(1, 0.1, 1));

  this->passMatrices();

  ground_base->render();
}

void Assignment2Scene::render_ice() {

  // Render ice
  this->model = mat4(1.0f);
  this->model = glm::translate(model, this->ice_center_loc);
  this->model = glm::scale(model, glm::vec3(1, 1, 1));

  this->passMatrices();

  // GL_ACTIVE_TEXTURE = 0
  this->activeShaderProgram->setUniform("envCubeMap", 0);

  MaterialProperties iceMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  ice->render();
}

void Assignment2Scene::render_objects(glm::mat4 view_matrix, bool do_render_ice,
                                      bool do_render_ice_fbo) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->basicShadingProgram.use();
  this->activeShaderProgram = &this->basicShadingProgram;

  this->view = view_matrix;

  this->render_ground();

  if (do_render_ice_fbo) {
    this->iceShadingProgram.use();
    this->activeShaderProgram = &this->iceShadingProgram;
    this->envMapFBOObject->bindCubeMapTexture();
  }
  if (do_render_ice)
    this->render_ice();
}

void Assignment2Scene::render() {

  // Render FBOs
  glEnable(GL_DEPTH_TEST);

  this->projection = this->envMapFBOObject->getProjectionMatrix();

  this->envMapFBOObject->bindCubeMapFBO(POSITIVE_X, false);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), false);
  this->envMapFBOObject->bindCubeMapFBO(NEGATIVE_X, false);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), false);
  this->envMapFBOObject->bindCubeMapFBO(POSITIVE_Y, false);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), false);
  this->envMapFBOObject->bindCubeMapFBO(NEGATIVE_Y, false);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), false);
  this->envMapFBOObject->bindCubeMapFBO(POSITIVE_Z, false);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), false);
  this->envMapFBOObject->bindCubeMapFBO(NEGATIVE_Z, false);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), false);

  // Load normal data
  //  this->envMapFBOObject->bindFBO(POSITIVE_X, true);
  //  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  //  this->envMapFBOObject->bindFBO(NEGATIVE_X, true);
  //  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  //  this->envMapFBOObject->bindFBO(POSITIVE_Y, true);
  //  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  //  this->envMapFBOObject->bindFBO(NEGATIVE_Y, true);
  //  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  //  this->envMapFBOObject->bindFBO(POSITIVE_Z, true);
  //  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  //  this->envMapFBOObject->bindFBO(NEGATIVE_Z, true);
  //  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);

  this->envMapFBOObject->unbindCubeMapFBO();

  this->projection = this->main_projection;

  this->render_objects(this->camera->getViewMatrix(), true, true);
}
// START UTILITIES
void Assignment2Scene::passMatrices() {

  if (this->activeShaderProgram == nullptr) {
    fprintf(stderr, "Could not send matrices, no active shader program set!\n");
    return;
  }

  mat4 mvMat = this->view * model;
  mat4 normalMat4 = glm::transpose(glm::inverse(model));
  mat3 normalMatrix =
      mat3(vec3(normalMat4[0]), vec3(normalMat4[1]), vec3(normalMat4[2]));

  this->activeShaderProgram->setUniform("ModelMatrix", model);
  this->activeShaderProgram->setUniform("ModelViewMatrix", mvMat);
  this->activeShaderProgram->setUniform("ProjectionMatrix", projection);
  this->activeShaderProgram->setUniform("NormalMatrix", normalMatrix);
  this->activeShaderProgram->setUniform("MVP", projection * mvMat);

  this->activeShaderProgram->setUniform("ViewportMatrix", this->view);
}

void Assignment2Scene::compileShaderPrograms() {

  this->basicShadingProgram.compileShader("shader/basic/basic.vs",
                                          GLSLShader::VERTEX);
  this->basicShadingProgram.compileShader("shader/basic/basic.fs",
                                          GLSLShader::FRAGMENT);
  this->basicShadingProgram.link();

  this->iceShadingProgram.compileShader("shader/ice/ice.vs",
                                        GLSLShader::VERTEX);
  this->iceShadingProgram.compileShader("shader/ice/ice.fs",
                                        GLSLShader::FRAGMENT);
  this->iceShadingProgram.link();
}

Assignment2Scene::~Assignment2Scene() {
  delete this->ground_plane;
  delete this->ground_base;
  delete this->ice;

  delete this->camera;
  delete this->envMapFBOObject;
}

void Assignment2Scene::handleCursorPositionEvent(glm::vec2 cursorPosition) {

  // if mouse hasn't moved in the window, prevent camera from flipping out
  if (std::fabs(mousePosition.x - MOUSE_UNINITIALIZED) <= 0.000001f) {

    mousePosition = cursorPosition;
  }
  float dX = min((cursorPosition.x - mousePosition.x), windowDimensions.x);
  float dY = min((cursorPosition.y - mousePosition.y), windowDimensions.y);

  this->camera->rotate(dX * 0.005f, dY * 0.005f);

  this->mousePosition = cursorPosition;
}

void Assignment2Scene::addColorToObject(TriangleMesh *object,
                                        const std::vector<glm::vec3> &colors) {
  // Convert from vec3 array to strided array
  std::vector<GLfloat> stridedColors;
  for (vec3 color : colors) {
    stridedColors.push_back(color.x);
    stridedColors.push_back(color.y);
    stridedColors.push_back(color.z);
  }

  GLuint colorVBO = 0; // Temporary value
  try {
    colorVBO = this->objectVaoColorVboMap.at(object->getVao());
  } catch (const std::out_of_range &e) {
    // If VBO value does not exist already, create it
    glGenBuffers(1, &colorVBO);
    this->objectVaoColorVboMap[object->getVao()] = colorVBO;
  }

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, stridedColors.size() * sizeof(GLfloat),
               stridedColors.data(), GL_STATIC_DRAW);

  // The count of defined (and set) VAOs in the object
  // Vertex Position, Normal, and (optionally) tex coord, tangents
  GLuint vaoCount = 2;
  if (object->getTcBuffer() != 0)
    vaoCount += 2;

  glBindVertexArray(object->getVao());

  glVertexAttribPointer(vaoCount, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  // Set location to non-predefined location
  glEnableVertexAttribArray(vaoCount);

  glBindVertexArray(0);
}

void Assignment2Scene::addColorToObject(TriangleMesh *object, glm::vec3 color) {
  std::vector<vec3> colors;
  for (int i = 0; i < object->getNumVerts(); i++)
    colors.emplace_back(color);
  this->addColorToObject(object, colors);
}

void Assignment2Scene::resize(int w, int h) {
  glViewport(0, 0, w, h);
  this->width = w;
  this->height = h;
  float aspectRatio = (float)w / (float)h;
  this->main_projection =
      glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
}

void Assignment2Scene::computeActiveMatrices() {

  this->projection = glm::perspective(glm::radians(60.0f),
                                      (float)width / height, 1.0f, 1000.0f);
}
