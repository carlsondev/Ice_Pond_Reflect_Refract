//
// Created by bcarl on 4/29/2023.
//

#include "env_fbo_object.h"

#include <iostream>

void EnvironmentMapFBOObject::bindFBO(DIRECTION direction) {

  // Bind texture
  // Assumes uniforms for images are texture_posx = 0, texture_negx = 1, etc.
  glActiveTexture(GL_TEXTURE0 + direction);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[direction]);

  // bind FBO
  glBindFramebuffer(GL_FRAMEBUFFER, fboArray[direction]);
}

void EnvironmentMapFBOObject::generateFBOs(int width, int height) {
  this->generateFBO(POSITIVE_X, width, height);
  this->generateFBO(NEGATIVE_X, width, height);
  this->generateFBO(POSITIVE_Y, width, height);
  this->generateFBO(NEGATIVE_Y, width, height);
  this->generateFBO(POSITIVE_Z, width, height);
  this->generateFBO(NEGATIVE_Z, width, height);
}

void EnvironmentMapFBOObject::generateFBO(DIRECTION direction, int width,
                                          int height) {

  // Generate FBO
  glGenFramebuffers(1, &fboArray[direction]);
  glBindFramebuffer(GL_FRAMEBUFFER, fboArray[direction]);

  // Generate texture
  glGenTextures(1, &fboTextureArray[direction]);
  glBindTexture(GL_TEXTURE_2D, fboTextureArray[direction]);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);

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
void EnvironmentMapFBOObject::cleanup() {

  for (int i = 0; i < 6; i++) {
    glDeleteFramebuffers(1, &fboArray[i]);
    glDeleteTextures(1, &fboTextureArray[i]);
  }
}
