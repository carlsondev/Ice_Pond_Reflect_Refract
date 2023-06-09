#include "cookbookogl.h"
#include "glutils.h"
#include "scene.h"
#include <GLFW/glfw3.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

class SceneRunner {
private:
  int fbw, fbh;
  int lastTime;
  int nFrames;
  std::string wTitle;
  bool debug; // Set true to enable debug messages

public:
  // Moved to public for external options
  GLFWwindow *window;

  SceneRunner(const std::string &windowTitle, int width = WIN_WIDTH,
              int height = WIN_HEIGHT, int samples = 0)
      : debug(true) {
    wTitle = windowTitle;
    // Initialize GLFW
    if (!glfwInit())
      exit(EXIT_FAILURE);

#ifdef __APPLE__
    // Select OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    // Select OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    if (debug)
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    if (samples > 0) {
      glfwWindowHint(GLFW_SAMPLES, samples);
    }

    // Open the window
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, windowTitle.c_str(), NULL,
                              NULL);
    if (!window) {
      std::cerr << "Unable to create OpenGL context." << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // Get framebuffer size
    glfwGetFramebufferSize(window, &fbw, &fbh);

    // Load the OpenGL functions.
    if (!gladLoadGL()) {
      exit(-1);
    }

    GLUtils::dumpGLInfo();

    // Initialization
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
#ifndef __APPLE__
    if (debug) {
      glDebugMessageCallback(GLUtils::debugCallback, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                            GL_TRUE);
      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                           GL_DEBUG_SEVERITY_NOTIFICATION, -1,
                           "Start debugging");
    }
#endif
  }

  int run(std::unique_ptr<Scene> scene) {
    // Enter the main loop
    mainLoop(window, std::move(scene));

#ifndef __APPLE__
    if (debug)
      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1,
                           GL_DEBUG_SEVERITY_NOTIFICATION, -1, "End debug");
#endif

    // Close window and terminate GLFW
    glfwTerminate();

    // Exit program
    return EXIT_SUCCESS;
  }

  static std::string
  parseCLArgs(int argc, char **argv,
              std::map<std::string, std::string> &sceneData) {
    if (argc < 2) {
      printHelpInfo(argv[0], sceneData);
      exit(EXIT_FAILURE);
    }

    std::string recipeName = argv[1];
    auto it = sceneData.find(recipeName);
    if (it == sceneData.end()) {
      printf("Unknown recipe: %s\n\n", recipeName.c_str());
      printHelpInfo(argv[0], sceneData);
      exit(EXIT_FAILURE);
    }

    return recipeName;
  }

private:
  static void printHelpInfo(const char *exeFile,
                            std::map<std::string, std::string> &sceneData) {
    printf("Usage: %s recipe-name\n\n", exeFile);
    printf("Recipe names: \n");
    for (auto it : sceneData) {
      printf("  %11s : %s\n", it.first.c_str(), it.second.c_str());
    }
  }

  void showFPS(GLFWwindow *window) {
    double currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    char ss[500] = {};
    nFrames++;
    if (delta >= 1.0) { // If last update was more than 1 sec ago
      double fps = ((double)(nFrames)) / delta;
      sprintf(ss, "%s running at %lf FPS.", wTitle.c_str(), fps);
      glfwSetWindowTitle(window, ss);
      nFrames = 0;
      lastTime = currentTime;
    }
  }

  void mainLoop(GLFWwindow *window, std::unique_ptr<Scene> scene) {
    lastTime = 0;
    nFrames = 0;
    scene->setDimensions(fbw, fbh);
    scene->initScene();
    scene->resize(fbw, fbh);
    double lastTime = 0;
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window) &&
           !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      GLUtils::checkForOpenGLError(__FILE__, __LINE__);
      // double time = glfwGetTime();
      // double deltaTime = time-lastTime;
      // if (deltaTime>=1/120.0) {
      //     lastTime = time;
      scene->update(float(glfwGetTime()));
      scene->render();
      // showFPS(window); Disabled to modify title personally
      glfwSwapBuffers(window);
      //}

      glfwPollEvents();
      int state = glfwGetKey(window, GLFW_KEY_SPACE);
      if (state == GLFW_PRESS)
        scene->animate(!scene->animating());
    }
  }
};
