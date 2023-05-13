# Adv. Computer Graphics Final Project

## Build Information

### CMakelists.txt (Windows)

```
cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED ON)

project (ice_pond)

if(MSVC)
    add_definitions(/DNOMINMAX)
endif()

if(UNIX)
    message(STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message(STATUS "Setting GLVND option.")
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package( OpenGL REQUIRED )

include_directories( ingredients )
include_directories( utilities )

add_subdirectory( ingredients )
add_subdirectory( utilities )
add_subdirectory( src )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/media DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
```

### CMakelists.txt (Linux)

```
cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED ON)
set (glm_DIR /usr/local/lib/cmake/glm/glm)
set (glfw3_DIR /usr/local/lib/cmake/glfw3/glfw3)

project (ice_pond)

if(MSVC)
  add_definitions(/DNOMINMAX)
endif()

if(UNIX)
    message(STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message(STATUS "Setting GLVND option.")
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package( /usr/local/lib/cmake/glm/glm REQUIRED)
find_package( /usr/local/lib/cmake/glfw3/glfw3 REQUIRED)
find_package( OpenGL REQUIRED )

include_directories( ingredients )
include_directories( utilities )

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

add_subdirectory( ingredients )
add_subdirectory( utilities )
add_subdirectory( chapter05 )
add_subdirectory( src )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/media DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
```


## Control Information

### Keyboard

* `C`: Toggle crack model
* `W/S`: Move teapot in the Z direction
* `A/D`: Move teapot in the X direction
* `Up/Down`: Move teapot in the Y direction
* `Escape`: Exit program

### Mouse

* `Pan`: Move around the look at point
* `Scroll`: Zoom in and out