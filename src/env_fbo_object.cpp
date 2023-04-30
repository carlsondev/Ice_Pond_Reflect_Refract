//
// Created by bcarl on 4/29/2023.
//

#include "env_fbo_object.h"

#include <iostream>

EnvironmentMapFBOObject::EnvironmentMapFBOObject(glm::vec3 position,
                                                 glm::vec2 windowDims) {
  this->windowDims = windowDims;
  this->object_position = position;

  this->generateFBO(POSITIVE_X);
  this->generateFBO(NEGATIVE_X);
  this->generateFBO(POSITIVE_Y);
  this->generateFBO(NEGATIVE_Y);
  this->generateFBO(POSITIVE_Z);
  this->generateFBO(NEGATIVE_Z);

  this->internal_camera = new BasicCamera();
  this->internal_camera->setPosition(position);
}

void EnvironmentMapFBOObject::bindEnvFBO(DIRECTION direction) {

  if (direction == POSITIVE_Y || direction == NEGATIVE_Y) {
    // Set up vector to look at vertical points
    this->internal_camera->setUpVector({0.0f, 0.0f, 1.0f});
  } else {
    // Set up vector to look at horizontal points
    this->internal_camera->setUpVector({0.0f, 1.0f, 0.0f});
  }

  switch (direction) {
  case POSITIVE_X:
    this->internal_camera->setLookAtPoint(this->object_position +
                                          glm::vec3(1.0f, 0.0f, 0.0f));
    break;
  case NEGATIVE_X:
    this->internal_camera->setLookAtPoint(this->object_position +
                                          glm::vec3(-1.0f, 0.0f, 0.0f));
    break;
  case POSITIVE_Y:
    this->internal_camera->setLookAtPoint(this->object_position +
                                          glm::vec3(0.0f, 1.0f, 0.0f));
    break;
  case NEGATIVE_Y:
    this->internal_camera->setLookAtPoint(this->object_position +
                                          glm::vec3(0.0f, -1.0f, 0.0f));
    break;
  case POSITIVE_Z:
    this->internal_camera->setLookAtPoint(this->object_position +
                                          glm::vec3(0.0f, 0.0f, 1.0f));
    break;
  case NEGATIVE_Z:
    this->internal_camera->setLookAtPoint(this->object_position +
                                          glm::vec3(0.0f, 0.0f, -1.0f));
    break;
  }

  // Bind texture
  // Assumes uniforms for images are texture_posx = 0, texture_negx = 1, etc.
  glActiveTexture(GL_TEXTURE0 + direction);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[direction]);

  // bind FBO
  glBindFramebuffer(GL_FRAMEBUFFER, fboArray[direction]);
}

void EnvironmentMapFBOObject::generateFBO(DIRECTION direction) {

  // Generate FBO
  glGenFramebuffers(1, &fboArray[direction]);
  glBindFramebuffer(GL_FRAMEBUFFER, fboArray[direction]);

  // Generate texture
  glGenTextures(1, &fboTextureArray[direction]);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[direction]);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->windowDims.x, this->windowDims.y,
               0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Attach texture to FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         fboTextureArray[direction], 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

EnvironmentMapFBOObject::~EnvironmentMapFBOObject() {
  for (int i = 0; i < 6; i++) {
    glDeleteFramebuffers(1, &fboArray[i]);
    glDeleteTextures(1, &fboTextureArray[i]);
  }

  delete this->internal_camera;
}
