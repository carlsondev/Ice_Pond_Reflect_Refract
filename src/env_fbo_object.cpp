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

/*
 *switch (direction) {
case POSITIVE_X:
  this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  this->internal_camera->setPhi(glm::half_pi<GLfloat>());
  break;
case NEGATIVE_X:
  this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  this->internal_camera->setPhi(-glm::half_pi<GLfloat>());
  break;
case POSITIVE_Y:
  this->internal_camera->setTheta(glm::pi<GLfloat>()); // Maybe
  this->internal_camera->setPhi(0);
  break;
case NEGATIVE_Y:
  this->internal_camera->setTheta(0); // Maybe
  this->internal_camera->setPhi(glm::pi<GLfloat>());
  break;
case POSITIVE_Z:
  this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  this->internal_camera->setPhi(glm::pi<GLfloat>());
  break;
case NEGATIVE_Z:
  this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  this->internal_camera->setPhi(0);
  break;
}
 */

void EnvironmentMapFBOObject::bindEnvFBO(DIRECTION direction) {

  if (direction == POSITIVE_Y) {
    // Set up vector to look at vertical points
    this->internal_camera->setUpVector(glm::vec3(0.0f, 0.0f, -1.0f));
  } else if (direction == NEGATIVE_Y) {
    this->internal_camera->setUpVector(glm::vec3(0.0f, 0.0f, 1.0f));
  } else {
    // Set up vector to look at horizontal points
    this->internal_camera->setUpVector(glm::vec3(0.0f, 1.0f, 0));
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

  //  switch (direction) {
  //  case POSITIVE_X:
  //    this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  //    this->internal_camera->setPhi(glm::half_pi<GLfloat>());
  //    break;
  //  case NEGATIVE_X:
  //    this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  //    this->internal_camera->setPhi(-glm::half_pi<GLfloat>());
  //    break;
  //  case POSITIVE_Y:
  //    this->internal_camera->setTheta(glm::pi<GLfloat>()); // Maybe
  //    this->internal_camera->setPhi(0);
  //    break;
  //  case NEGATIVE_Y:
  //    this->internal_camera->setTheta(0); // Maybe
  //    this->internal_camera->setPhi(glm::pi<GLfloat>());
  //    break;
  //  case POSITIVE_Z:
  //    this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  //    this->internal_camera->setPhi(glm::pi<GLfloat>());
  //    break;
  //  case NEGATIVE_Z:
  //    this->internal_camera->setTheta(glm::half_pi<GLfloat>());
  //    this->internal_camera->setPhi(0);
  //    break;
  //  }
  this->internal_camera->recomputeOrientation();

  // Bind texture
  // Assumes uniforms for images are texture_posx = 0, texture_negx = 1, etc.
  glActiveTexture(GL_TEXTURE0 + direction);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[direction]);

  // bind FBO
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboArray[direction]);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void EnvironmentMapFBOObject::bindAllFBOTextures() {
  glActiveTexture(GL_TEXTURE0 + POSITIVE_X);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[POSITIVE_X]);

  glActiveTexture(GL_TEXTURE0 + NEGATIVE_X);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[NEGATIVE_X]);

  glActiveTexture(GL_TEXTURE0 + POSITIVE_Y);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[POSITIVE_Y]);

  glActiveTexture(GL_TEXTURE0 + NEGATIVE_Y);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[NEGATIVE_Y]);

  glActiveTexture(GL_TEXTURE0 + POSITIVE_Z);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[POSITIVE_Z]);

  glActiveTexture(GL_TEXTURE0 + NEGATIVE_Z);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[NEGATIVE_Z]);
}

void EnvironmentMapFBOObject::generateFBO(DIRECTION direction) {

  // Generate FBO
  glGenFramebuffers(1, &fboArray[direction]);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboArray[direction]);

  // Generate render buffer
  glGenRenderbuffers(1, &fboRBArray[direction]);
  glBindRenderbuffer(GL_RENDERBUFFER, fboRBArray[direction]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32,
                        this->windowDims.x, this->windowDims.y);

  // Generate texture
  glActiveTexture(GL_TEXTURE0 + direction);
  glGenTextures(1, &fboTextureArray[direction]);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[direction]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, this->windowDims.x,
                 this->windowDims.y);

  // Attach texture to FBO
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, fboTextureArray[direction], 0);

  // Attach render buffer to FBO
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, fboRBArray[direction]);

  // Check status
  if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;

  // Default state
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

EnvironmentMapFBOObject::~EnvironmentMapFBOObject() {
  for (int i = 0; i < 6; i++) {
    glDeleteFramebuffers(1, &fboArray[i]);
    glDeleteRenderbuffers(1, &fboRBArray[i]);
    glDeleteTextures(1, &fboTextureArray[i]);
  }

  delete this->internal_camera;
}
