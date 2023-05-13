//
// Created by bcarl on 4/29/2023.
//

#ifndef ICE_POND_BASICCAMERA_HPP
#define ICE_POND_BASICCAMERA_HPP

#include <Camera.hpp>
#include <cstdio>

class BasicCamera : public Camera {

public:
  BasicCamera() : Camera() {}

  void set_pitch_yaw(float pitch_new, float yaw_new) {
    this->pitch = pitch_new;
    this->yaw = yaw_new;
    computePitchRollViewMatrix();
  }

  void recomputeOrientation() override {
    // compute direction vector based on spherical to cartesian conversion
    _direction.x = sinf(_theta) * sinf(_phi) * _radius;
    _direction.y = -cosf(_phi) * _radius;
    _direction.z = -cosf(_theta) * sinf(_phi) * _radius;
  }

  glm::mat4 getProjectionMatrix() {
    float aspect_ratio = 1.0;
    return glm::perspective(glm::radians(90.0f), aspect_ratio, 0.3f, 100.0f);
  }

private:
  float pitch;
  float yaw;
  /// \desc steps forward along the camera's view
  /// \param movementFactor scaling factor for distance to move
  void moveForward(GLfloat movementFactor) override{

  };

  /// \desc steps backward along the camera's view
  /// \param movementFactor scaling factor for distance to move
  void moveBackward(GLfloat movementFactor) override{

  };

  void computePitchRollViewMatrix() {
    _viewMatrix = glm::mat4(1.0);
    _viewMatrix =
        glm::rotate(_viewMatrix, glm::radians(180.0f), glm::vec3(0, 0, 1));
    _viewMatrix = glm::rotate(_viewMatrix, glm::radians(pitch), glm::vec3(1, 0,
                                                                       0));
    _viewMatrix = glm::rotate(_viewMatrix, glm::radians(yaw), glm::vec3(0, 1,
                                                                       0));
    _viewMatrix = glm::translate(_viewMatrix, -_position);
  }
};

#endif // ICE_POND_BASICCAMERA_HPP
