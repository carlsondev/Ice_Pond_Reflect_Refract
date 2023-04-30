//
// Created by bcarl on 4/29/2023.
//

#ifndef ICE_POND_ENV_FBO_OBJECT_H
#define ICE_POND_ENV_FBO_OBJECT_H

#include "glad/glad.h"

enum DIRECTION{
  POSITIVE_X = 0,
  NEGATIVE_X = 1,
  POSITIVE_Y = 2,
  NEGATIVE_Y = 3,
  POSITIVE_Z = 4,
  NEGATIVE_Z = 5
};

class EnvironmentMapFBOObject {

public:

  void generateFBOs(int width, int height);

  void bindFBO(DIRECTION direction);

  void cleanup();


private:

  GLuint fboArray[6];
  GLuint fboTextureArray[6];

  void generateFBO(DIRECTION direction, int width, int height);

};

#endif // ICE_POND_ENV_FBO_OBJECT_H
