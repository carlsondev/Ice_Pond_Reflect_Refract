//
// Created by bcarl on 2/9/2023.
//

#include "ice_pond.h"
#include "texture.h"

using namespace glm;

// START OF RENDERING FUNCTIONS
void IcePond::render_ground() {

  // Render ground plane
  this->model = mat4(1.0f);
  this->model = glm::translate(model, vec3(0.0f, -10.0f, 0.0f));
  this->model = glm::scale(model, glm::vec3(1, 1, 1));

  this->passMatrices();

  MaterialProperties groundMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  ground_plane->render();

  // Render ground base
  this->model = mat4(1.0f);
  this->model = glm::translate(model, vec3(0.0f, -20.0f, 0.0f));
  this->model = glm::scale(model, glm::vec3(1, 0.1, 1));

  this->passMatrices();

  ground_base->render();
}

void IcePond::render_ice() {

  // Render ice
  this->model = mat4(1.0f);
  this->model =
      glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
  this->model = glm::translate(model, this->ice_center_loc);
  // this->model = glm::scale(model, glm::vec3(5, 1, 5));

  this->passMatrices();

  // GL_ACTIVE_TEXTURE = 0
  this->activeShaderProgram->setUniform("envCubeMap", 0);
  this->activeShaderProgram->setUniform("normCubeMap", 1);

  MaterialProperties iceMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  iceTeapot->render();
}

void IcePond::render_objects(glm::mat4 view_matrix, bool do_render_ice,
                             bool do_render_ice_fbo) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->view = view_matrix;
  // this->render_ground();

  this->activeShaderProgram = &this->skyProg;
  // Draw sky
  skyProg.use();
  model = mat4(1.0f);
  this->passMatrices();
  sky->render();

  if (do_render_ice_fbo) {
    this->iceShadingProgram.use();
    this->activeShaderProgram = &this->iceShadingProgram;
    this->envMapFBOObject->bindCubeMapTexture();
  } else {
    this->basicShadingProgram.use();
    this->activeShaderProgram = &this->basicShadingProgram;
  }
  if (do_render_ice)
    this->render_ice();
}

void IcePond::render() {

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

  this->envMapFBOObject->bindCubeMapFBO(POSITIVE_X, true);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  this->envMapFBOObject->bindCubeMapFBO(NEGATIVE_X, true);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  this->envMapFBOObject->bindCubeMapFBO(POSITIVE_Y, true);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  this->envMapFBOObject->bindCubeMapFBO(NEGATIVE_Y, true);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  this->envMapFBOObject->bindCubeMapFBO(POSITIVE_Z, true);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);
  this->envMapFBOObject->bindCubeMapFBO(NEGATIVE_Z, true);
  this->render_objects(this->envMapFBOObject->getViewMatrix(), true);

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
