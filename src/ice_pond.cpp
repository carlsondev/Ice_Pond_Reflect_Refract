//
// Created by bcarl on 2/9/2023.
//

#include "ice_pond.h"

#include <cmath>
#include <cstdio>

using namespace glm;

static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                        int mods) {
  if (action != GLFW_RELEASE) {
    return;
  }

  auto scene = (Assignment2Scene *)glfwGetWindowUserPointer(window);
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

  glfwSetKeyCallback(this->currentWindow, keyCallback);

  glfwSetWindowUserPointer(this->currentWindow, this);

  glfwSetCursorPosCallback(window, mousePositionCallback);

  glfwSetScrollCallback(window, mouseScrollCallback);
}

void Assignment2Scene::initScene() {
  this->compileShaderPrograms();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int width, height = 0;

  glfwGetWindowSize(this->currentWindow, &width, &height);

  this->camera = new ArcballCam(2.0f, 100.0f);
  this->envMapFBOObject = new EnvironmentMapFBOObject(this->ice_center_loc,
                                                      glm::vec2(width, height));

  this->ground_plane = new Plane(100, 100, 1, 1);
  this->ground_base = new Cube(100.0f); // Scale smaller vertically
  this->ice = new Cube(5.0f);

  // Add green color to ground plane
  this->addColorToObject(this->ground_plane, {0.49, 0.78, 0.47});

  // Add brown color to ground base
  this->addColorToObject(this->ground_base,
                         {78.0 / 255.0, 53.0 / 255.0, 36.0 / 255.0});

  // Add blue color to ice
  this->addColorToObject(this->ice,
                         {102.0 / 255.0, 153.0 / 255.0, 204.0 / 255.0});
  this->basicShadingProgram.use();
  this->activeShaderProgram = &this->basicShadingProgram;
}

void Assignment2Scene::computeActiveMatrices() {

  this->projection = glm::perspective(glm::radians(60.0f),
                                      (float)width / height, 1.0f, 1000.0f);
}

void Assignment2Scene::update(float t) {
  this->systemTime = t;
  this->computeActiveMatrices();
}

void Assignment2Scene::render_ground() {
  this->basicShadingProgram.use();
  this->activeShaderProgram = &this->basicShadingProgram;

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
  this->iceShadingProgram.use();
  this->activeShaderProgram = &this->iceShadingProgram;

  // Render ice
  this->model = mat4(1.0f);
  this->model = glm::translate(model, this->ice_center_loc);
  this->model = glm::scale(model, glm::vec3(1, 1, 1));

  this->passMatrices();

  MaterialProperties iceMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  ice->render();
}

void Assignment2Scene::render_objects(glm::mat4 view_matrix) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->view = view_matrix;

  this->render_ground();

  this->render_ice();
}

void Assignment2Scene::render() {

  // Render FBOs
  this->envMapFBOObject->bindEnvFBO(POSITIVE_X);
  this->render_objects(this->envMapFBOObject->getViewMatrix());
  this->envMapFBOObject->bindEnvFBO(NEGATIVE_X);
  this->render_objects(this->envMapFBOObject->getViewMatrix());
  this->envMapFBOObject->bindEnvFBO(POSITIVE_Y);
  this->render_objects(this->envMapFBOObject->getViewMatrix());
  this->envMapFBOObject->bindEnvFBO(NEGATIVE_Y);
  this->render_objects(this->envMapFBOObject->getViewMatrix());
  this->envMapFBOObject->bindEnvFBO(POSITIVE_Z);
  this->render_objects(this->envMapFBOObject->getViewMatrix());

  this->envMapFBOObject->unbindFBO();
  this->render_objects(this->camera->getViewMatrix());
}

void Assignment2Scene::resize(int w, int h) {
  glViewport(0, 0, w, h);
  this->width = w;
  this->height = h;
  float aspectRatio = (float)w / (float)h;
  this->projection =
      glm::perspective(glm::radians(60.0f), aspectRatio, 0.3f, 100.0f);
}

void Assignment2Scene::passMatrices() {

  if (this->activeShaderProgram == nullptr) {
    fprintf(stderr, "Could not send matrices, no active shader program set!\n");
    return;
  }

  mat4 mvMat = this->view * model;

  this->activeShaderProgram->setUniform("ModelMatrix", model);
  this->activeShaderProgram->setUniform("ModelViewMatrix", mvMat);
  this->activeShaderProgram->setUniform("ProjectionMatrix", projection);
  this->activeShaderProgram->setUniform(
      "NormalMatrix",
      glm::mat3(vec3(mvMat[0]), vec3(mvMat[1]), vec3(mvMat[2])));
  this->activeShaderProgram->setUniform("MVP", projection * mvMat);

  this->activeShaderProgram->setUniform("ViewportMatrix", this->view);

  this->activeShaderProgram->setUniform("TessLevel", 10);
  this->activeShaderProgram->setUniform("LineWidth", 0.5f);
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
}

void Assignment2Scene::handleCursorPositionEvent(glm::vec2 cursorPosition) {

  // if mouse hasn't moved in the window, prevent camera from flipping out
  if (std::fabs(mousePosition.x - MOUSE_UNINITIALIZED) <= 0.000001f) {
    mousePosition = cursorPosition;
  }
  this->camera->rotate((cursorPosition.x - mousePosition.x) * 0.005f,
                       (cursorPosition.y - mousePosition.y) * 0.005f);

  this->mousePosition = cursorPosition;
}
