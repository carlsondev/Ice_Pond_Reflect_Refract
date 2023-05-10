set(target ice_pond)
set( project_SOURCES
        ice_pond.cpp
        main.cpp
        env_fbo_object.cpp env_fbo_object.h ice_pond_utils.cpp)

add_executable( ${target} ${project_SOURCES} )

target_compile_definitions(${target}
        PRIVATE
        GLFW_INCLUDE_NONE
        )

target_link_libraries( ${target}
        PRIVATE
        ingredients
        utilities
        glfw3
        ${OPENGL_gl_LIBRARY}
        )

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})