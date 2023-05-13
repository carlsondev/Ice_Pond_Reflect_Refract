//
// Created by bcarl on 4/29/2023.
//

#include "env_fbo_object.h"

#include <iostream>

EnvironmentMapFBOObject::EnvironmentMapFBOObject(glm::vec3 position) {
  this->object_position = position;

  this->generateCubeMapFBO();

  this->internal_camera = new BasicCamera();
  this->internal_camera->setPosition(position);
}

// 9:00
void EnvironmentMapFBOObject::rotateCameraToDirection(DIRECTION direction) {

  switch (direction) {
  case POSITIVE_X:
    this->internal_camera->set_pitch_yaw(0, 90);
    break;
  case NEGATIVE_X:
    this->internal_camera->set_pitch_yaw(0, -90);
    break;
  case POSITIVE_Y:
    this->internal_camera->set_pitch_yaw(-90, 180);
    break;
  case NEGATIVE_Y:
    this->internal_camera->set_pitch_yaw(90, 180);
    break;
  case POSITIVE_Z:
    this->internal_camera->set_pitch_yaw(0, 180);
    break;
  case NEGATIVE_Z:
    this->internal_camera->set_pitch_yaw(0, 0);
    break;
  }

  this->internal_camera->recomputeOrientation();
}

void EnvironmentMapFBOObject::bindCubeMapFBO(DIRECTION direction,
                                             bool bind_norm) {
  // Bind env texture
  // Assumes uniforms for images are texture_posx = 0, texture_negx = 1, etc.
  this->rotateCameraToDirection(direction);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cubeMapFBO);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X + direction,
                         cubeMapTexture, 0);

  GLuint drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_NONE};

  glDrawBuffers(2, drawBuffers);
}

void EnvironmentMapFBOObject::generateCubeMapFBO() {

  // Setup Cube Map Texture
  glGenTextures(1, &cubeMapTexture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Setup storage for Cube Map
  glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, CUBE_MAP_WIDTH,
                 CUBE_MAP_WIDTH);

  // Restore to default state
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  // Generate FBO
  glGenFramebuffers(1, &cubeMapFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cubeMapFBO);

  // 13:00
  // Attach cube map side texture to FBO
  for (int i = POSITIVE_X; i <= NEGATIVE_Z; i++) {
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture,
                           0);
  }

  // Generate render buffer
  glGenRenderbuffers(1, &renderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBE_MAP_WIDTH,
                        CUBE_MAP_WIDTH);

  // Attach render buffer to FBO
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, renderBuffer);

  // Check status
  if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;

  // Default state
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

EnvironmentMapFBOObject::~EnvironmentMapFBOObject() {
  for (int i = 0; i < 6; i++) {
    glDeleteTextures(1, &fboNormalTextureArray[i]);
  }

  delete this->internal_camera;
}
void EnvironmentMapFBOObject::bindCubeMapTexture() {
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
}
