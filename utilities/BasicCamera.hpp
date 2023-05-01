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

  void recomputeOrientation() override {
    // compute direction vector based on spherical to cartesian conversion
    _direction.x = sinf(_theta) * sinf(_phi) * _radius;
    _direction.y = -cosf(_phi) * _radius;
    _direction.z = -cosf(_theta) * sinf(_phi) * _radius;

    //    printf("Direction (%f, %f, %f)\n", _direction.x, _direction.y,
    //           _direction.z);
    //    printf("Position (%f, %f, %f)\n", _position.x, _position.y,
    //    _position.z); printf("LookAt (%f, %f, %f)\n", _lookAtPoint.x,
    //    _lookAtPoint.y,
    //           _lookAtPoint.z);

    _updateFreeCameraViewMatrix();
  }

private:
  /// \desc steps forward along the camera's view
  /// \param movementFactor scaling factor for distance to move
  void moveForward(GLfloat movementFactor) override{

  };

  /// \desc steps backward along the camera's view
  /// \param movementFactor scaling factor for distance to move
  void moveBackward(GLfloat movementFactor) override{

  };

  void _updateFreeCameraViewMatrix() {
    // setPosition(_lookAtPoint + _direction);
    computeViewMatrix();
  }
};

#endif // ICE_POND_BASICCAMERA_HPP
