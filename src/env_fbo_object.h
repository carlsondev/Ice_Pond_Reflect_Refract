//
// Created by bcarl on 4/29/2023.
//

#ifndef ICE_POND_ENV_FBO_OBJECT_H
#define ICE_POND_ENV_FBO_OBJECT_H

#include "glad/glad.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <BasicCamera.hpp>

enum DIRECTION {
  POSITIVE_X = 0,
  NEGATIVE_X = 1,
  POSITIVE_Y = 2,
  NEGATIVE_Y = 3,
  POSITIVE_Z = 4,
  NEGATIVE_Z = 5
};

class EnvironmentMapFBOObject {

public:
  EnvironmentMapFBOObject(glm::vec3 position);

  ~EnvironmentMapFBOObject();

  void bindCubeMapFBO(DIRECTION direction, bool bind_norm);

  void bindCubeMapTexture();

  inline void unbindCubeMapFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK_LEFT);
  }

  inline void setObjectPosition(glm::vec3 position) {
    this->object_position = position;
    this->internal_camera->setPosition(position);
  }

  glm::mat4 getViewMatrix() { return this->internal_camera->getViewMatrix(); }
  glm::mat4 getProjectionMatrix() {
    return this->internal_camera->getProjectionMatrix();
  }
  BasicCamera *internal_camera;

private:
  glm::vec3 object_position;

  GLuint envCubeMapTexture;
  GLuint normCubeMapTexture;

  GLuint cubeMapFBO;
  GLuint renderBuffer;

  const int CUBE_MAP_WIDTH = 512;

  void rotateCameraToDirection(DIRECTION direction);

  void generateCubeMapFBO(GLuint &cubeMapTexture);
};

#endif // ICE_POND_ENV_FBO_OBJECT_H
