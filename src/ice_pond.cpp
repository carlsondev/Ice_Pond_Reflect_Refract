//
// Created by bcarl on 2/9/2023.
//

#include "ice_pond.h"

using namespace glm;

// START OF RENDERING FUNCTIONS
void Assignment2Scene::render_ground() {

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

  // Render ice
  this->model = mat4(1.0f);
  this->model = glm::translate(model, this->ice_center_loc);
  this->model = glm::scale(model, glm::vec3(1, 1, 1));

  this->passMatrices();

  // GL_ACTIVE_TEXTURE = 0
  this->activeShaderProgram->setUniform("envCubeMap", 0);

  MaterialProperties iceMaterialProperties{
      {0.9f, 0.5f, 0.3f}, {0.9f, 0.5f, 0.3f}, {0.8f, 0.8f, 0.8f}, 1.0f};

  ice->render();
}

void Assignment2Scene::render_objects(glm::mat4 view_matrix, bool do_render_ice,
                                      bool do_render_ice_fbo) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  this->basicShadingProgram.use();
  this->activeShaderProgram = &this->basicShadingProgram;

  this->view = view_matrix;

  this->render_ground();

  if (do_render_ice_fbo) {
    this->iceShadingProgram.use();
    this->activeShaderProgram = &this->iceShadingProgram;
    this->envMapFBOObject->bindCubeMapTexture();
  }
  if (do_render_ice)
    this->render_ice();
}

void Assignment2Scene::render() {

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
