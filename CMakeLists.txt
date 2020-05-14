cmake_minimum_required(VERSION 3.10)
project(stereo)

set(CMAKE_CXX_STANDARD 11)

option(BUILD_DOCUMENTATION "Bulid the documentation with Doxygen" OFF)
option(STEREO_SHARED_LIBS "Build the library as shared (.dll/.so) instead of static (.lib/.a)" ON)


# Set the output directory for build artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Set the path to CMake scripts used for finding packages.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")


# Build the documentation
if (BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # request to configure the file
        message(STATUS "Doxygen build started")

        set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

        add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else (DOXYGEN_FOUND)
        message(WARNING "Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif (BUILD_DOCUMENTATION)


# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# GLEW
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# OpenVR
find_package(OpenVR REQUIRED)


# Shader Source
add_executable(shader_embedder src/tools/shaderembedder.cpp)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/shadersource.hpp
    COMMAND shader_embedder
        ${CMAKE_CURRENT_SOURCE_DIR}/include/shadersource.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment_shader.frag frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex_shader.vert vert
    DEPENDS shader_embedder
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment_shader.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex_shader.vert
    COMMENT "Generating shader header..."
)
add_custom_target(shader_header ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/shadersource.hpp)

# Stereo Library
set(STEREO_SOURCES
    src/stereo.cpp
    src/shader.cpp
    src/mesh.cpp
    src/framebuffer.cpp
)

set(STEREO_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(STEREO_INCLUDES
    PUBLIC ${STEREO_INCLUDE_DIR}
    PUBLIC ${GLEW_INCLUDE_DIRS}
    PUBLIC ${OPENGL_INCLUDE_DIRS}
    PUBLIC ${OPENVR_INCLUDE_DIR}
)

set(STEREO_LIBRARIES
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OpenVR_LIBRARY}
)

if (STEREO_SHARED_LIBS)
   add_library(stereo SHARED)
else()
   add_library(stereo STATIC)
endif()

target_sources(stereo PRIVATE ${STEREO_SOURCES})
add_dependencies(stereo shader_header)
target_include_directories(stereo ${STEREO_INCLUDES})
target_link_libraries(stereo ${STEREO_LIBRARIES})

# Set the prefix to use when printing logging information
target_compile_definitions(stereo INTERFACE LOG_SOURCE_MODULE="stereo" MAKEDLL="")


# Add example executable directories
add_subdirectory(example)


add_custom_command ( TARGET stereo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/actions.json"
        $<TARGET_FILE_DIR:stereo>
)

add_custom_command ( TARGET stereo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/gamepad_bindings.json"
        $<TARGET_FILE_DIR:stereo>
)
