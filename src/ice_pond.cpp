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

  if (key == GLFW_KEY_N) {
    scene->toggleNormalNoise();
  }
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

  this->camera = new ArcballCam(2.0f, 100.0f);

  this->groundPlane = new Plane(100, 100, 1, 1);

  // Add random green colors
  std::vector<vec3> groundColors;
  for (int i = 0; i < this->groundPlane->getNumVerts(); i++)
    groundColors.emplace_back(0.49, 0.78, 0.47);
  this->addColorToObject(this->groundPlane, groundColors);

  this->teapot = new TeapotPatch();

  this->isUsingNormalNoise = false;
  this->teapotShadingProgram.use();
  this->teapotShadingProgram.setUniform("UseNormalNoise", false);

  this->wireframeShadingProgram.use();
  this->activeShaderProgram = &this->wireframeShadingProgram;
}

void Assignment2Scene::computeActiveMatrices() {

  this->view = camera->getViewMatrix();
  this->projection = glm::perspective(glm::radians(60.0f),
                                      (float)width / height, 1.0f, 1000.0f);
}

void Assignment2Scene::update(float t) {
  this->systemTime = t;
  this->computeActiveMatrices();
}
void Assignment2Scene::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->wireframeShadingProgram.use();
  this->activeShaderProgram = &this->wireframeShadingProgram;

  this->model = mat4(1.0f);
  this->model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
  this->model = glm::scale(model, glm::vec3(1, 1, 1));

  this->passMatrices();

  MaterialProperties groundMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  groundPlane->render();

  this->renderTeapot();
}

void Assignment2Scene::renderTeapot() {

  this->teapotShadingProgram.use();
  this->activeShaderProgram = &this->teapotShadingProgram;

  this->model = mat4(1.0f);
  this->model = glm::translate(model, vec3(0.0f, 8.0f, 0.0f));
  this->model = glm::scale(model, glm::vec3(4, 4, 4));
  this->model = glm::rotate(model, glm::radians(-90.0f), vec3(1, 0, 0));

  this->passMatrices();

  this->teapot->render();
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

  mat4 mvMat = view * model;

  this->activeShaderProgram->setUniform("ModelMatrix", model);
  this->activeShaderProgram->setUniform("ModelViewMatrix", mvMat);
  this->activeShaderProgram->setUniform("ProjectionMatrix", projection);
  this->activeShaderProgram->setUniform(
      "NormalMatrix",
      glm::mat3(vec3(mvMat[0]), vec3(mvMat[1]), vec3(mvMat[2])));
  this->activeShaderProgram->setUniform("MVP", projection * mvMat);

  this->activeShaderProgram->setUniform("ViewportMatrix",
                                        this->camera->getViewMatrix());

  this->activeShaderProgram->setUniform("TessLevel", 10);
  this->activeShaderProgram->setUniform("LineWidth", 0.5f);
}

void Assignment2Scene::addColorToObject(TriangleMesh *object,
                                        std::vector<glm::vec3> colors) {
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

void Assignment2Scene::compileShaderPrograms() {

  this->basicShadingProgram.compileShader("shader/basic/basic.vs",
                                          GLSLShader::VERTEX);
  this->basicShadingProgram.compileShader("shader/basic/basic.fs",
                                          GLSLShader::FRAGMENT);
  this->basicShadingProgram.link();

  this->wireframeShadingProgram.compileShader("shader/wireframe/wireframe.vs",
                                              GLSLShader::VERTEX);
  this->wireframeShadingProgram.compileShader("shader/wireframe/wireframe.gs",
                                              GLSLShader::GEOMETRY);
  this->wireframeShadingProgram.compileShader("shader/wireframe/wireframe.fs",
                                              GLSLShader::FRAGMENT);
  this->wireframeShadingProgram.link();

  this->teapotShadingProgram.compileShader("shader/teapot/teapot.vs",
                                           GLSLShader::VERTEX);
  this->teapotShadingProgram.compileShader("shader/teapot/teapot.gs",
                                           GLSLShader::GEOMETRY);
  this->teapotShadingProgram.compileShader("shader/teapot/teapot.fs",
                                           GLSLShader::FRAGMENT);
  this->teapotShadingProgram.compileShader("shader/teapot/teapot.tcs",
                                           GLSLShader::TESS_CONTROL);
  this->teapotShadingProgram.compileShader("shader/teapot/teapot.tes",
                                           GLSLShader::TESS_EVALUATION);

  this->teapotShadingProgram.link();
}

Assignment2Scene::~Assignment2Scene() {
  delete this->groundPlane;
  delete this->teapot;
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
