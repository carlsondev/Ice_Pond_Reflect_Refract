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
  EnvironmentMapFBOObject(glm::vec3 position, glm::vec2 windowDims);

  ~EnvironmentMapFBOObject();

  void bindEnvFBO(DIRECTION direction);

  void bindAllFBOTextures();

  inline void unbindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK_LEFT);
  }

  inline void setObjectPosition(glm::vec3 position) {
    this->object_position = position;
    this->internal_camera->setPosition(position);
  }

  glm::mat4 getViewMatrix() { return this->internal_camera->getViewMatrix(); }
  BasicCamera *internal_camera;

private:
  GLuint fboArray[6];
  GLuint fboTextureArray[6];
  GLuint fboRBArray[6];
  glm::vec2 windowDims;
  glm::vec3 object_position;

  void generateFBO(DIRECTION direction);
};

#endif // ICE_POND_ENV_FBO_OBJECT_H
