//
// Created by bcarl on 2/10/2023.
//

#include <cstdio>

#include "scene.h"
#include "scenerunner.h"

#include "ice_pond.h"

int main(int argc, char *argv[]) {
  printf("Hello World!\n");

  SceneRunner runner("Ice Pond", 1920, 3413);

  std::unique_ptr<Scene> scene(new IcePond(runner.window));
  return runner.run(std::move(scene));
}