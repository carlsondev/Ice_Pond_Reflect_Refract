set(target utilities)

set(utilities_SOURCES
        ArcballCam.hpp
        Camera.hpp BasicCamera.hpp)

add_library(${target} STATIC ${utilities_SOURCES})

set_target_properties(${target} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${target} PUBLIC ../ingredients/glad/include)

if( UNIX AND NOT APPLE )
    target_link_libraries(${target} PUBLIC ${CMAKE_DL_LIBS})
endif()


