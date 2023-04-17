//
// Created by bcarl on 2/9/2023.
//

#ifndef ASSIGNMENT_1_ASSIGNMENT1_H
#define ASSIGNMENT_1_ASSIGNMENT1_H

#include "glslprogram.h"
#include "scene.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/vec4.hpp>

#include "plane.h"
#include "teapotpatch.h"
#include "trianglemesh.h"

#include "ArcballCam.hpp"

struct MaterialProperties {
  glm::vec3 ambientReflectivity;
  glm::vec3 diffuseReflectivity;
  glm::vec3 specularReflectivity;
  float shininess;
};

class Assignment2Scene : public Scene {

  // value off-screen to represent mouse has not begun interacting with
  // window yet
  static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
  GLFWwindow *currentWindow;

  GLSLProgram basicShadingProgram;
  GLSLProgram wireframeShadingProgram;
  GLSLProgram teapotShadingProgram;

  // The currently used shader program
  GLSLProgram *activeShaderProgram = nullptr;

  // Rendered objects
  Plane *groundPlane = nullptr;
  TeapotPatch *teapot = nullptr;

  glm::vec2 mousePosition;

  float systemTime = 0;
  bool isUsingNormalNoise = false;

  // Map from the object VAOs to the Color VBOs
  // Necessary because the Object VAOs are defined outside of this scope
  std::map<GLuint, GLuint> objectVaoColorVboMap;

  void compileShaderPrograms();

  // Sets view and projection matrix based on current data
  void computeActiveMatrices();

  void passMatrices();

  void renderTeapot();

  // Add color to "ingredients" object using a vector of colors for every
  // vertex.
  void addColorToObject(TriangleMesh *object, std::vector<glm::vec3> colors);

  // Generates a random float between 0.0 and 1.0
  float rand01() { return (float)std::rand() / (float)RAND_MAX; }

  // Fills a vector of 3d vectors with count random 3d vectors
  std::vector<glm::vec3> fillRandVec3Vector(int count) {
    std::vector<glm::vec3> outVec;
    for (int i = 0; i < count; i++)
      outVec.emplace_back(rand01(), rand01(), rand01());
    return outVec;
  }

public:
  explicit Assignment2Scene(GLFWwindow *window);

  ArcballCam *camera = nullptr;

  void initScene() override;
  void update(float t) override;
  void render() override;
  void resize(int width, int height) override;

  void setBasicShaderEnabled() {
    this->basicShadingProgram.use();
    this->activeShaderProgram = &this->basicShadingProgram;
  }

  void toggleNormalNoise() {
    this->isUsingNormalNoise = !this->isUsingNormalNoise;
    this->teapotShadingProgram.setUniform("UseNormalNoise",
                                          this->isUsingNormalNoise);
  }

  // Called by the cursor callback
  void handleCursorPositionEvent(glm::vec2 cursorPosition);

  ~Assignment2Scene() override;
};

#endif // ASSIGNMENT_1_ASSIGNMENT1_H
