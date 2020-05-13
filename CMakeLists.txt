cmake_minimum_required(VERSION 3.10)
project(stereo)

set(CMAKE_CXX_STANDARD 11)

option(BUILD_DOCUMENTATION "Bulid the documentation with Doxygen" OFF)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Pull down submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()


# Try to guess the platform architecture.
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PLATFORM 64)
else()
    set(PLATFORM 32)
endif()


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
if(WIN32)
    set(WINDOWS_PATH_SUFFIXES win${PLATFORM} Win${PLATFORM} x${PLATFORM})
endif()

find_library(OPENVR_LIBRARY
    NAMES
        openvr_api OpenVR
    PATHS
        libs/openvr/bin
        libs/openvr/lib
    PATH_SUFFIXES
        osx64
        linux64
        ${WINDOWS_PATH_SUFFIXES}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)
set(OPENVR_INCLUDE_DIR libs/openvr/headers)


# Resources
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/img
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

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
    src/stb_image.cpp
    src/stereo.cpp
    src/shader.cpp
    src/mesh.cpp
    src/texture.cpp
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
    ${OPENVR_LIBRARY}
)

add_library(stereo SHARED ${STEREO_SOURCES})
add_library(stereo-static STATIC ${STEREO_SOURCES})

add_dependencies(stereo shader_header)
add_dependencies(stereo-static shader_header)

target_include_directories(stereo ${STEREO_INCLUDES})
target_include_directories(stereo-static ${STEREO_INCLUDES})

target_link_libraries(stereo ${STEREO_LIBRARIES})
target_link_libraries(stereo-static ${STEREO_LIBRARIES})

# Set the prefix to use when printing logging information
target_compile_definitions(stereo INTERFACE LOG_SOURCE_MODULE="stereo")
target_compile_definitions(stereo-static INTERFACE LOG_SOURCE_MODULE="stereo")


# Add example executable directories
add_subdirectory(example)

# Copy the OpenVR DLL to the output directory
if(WIN32)
    set(OPENVR_DLL "${PROJECT_SOURCE_DIR}/libs/openvr/bin/win64/openvr_api.dll")
	message("Copying ${OPENVR_DLL} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    file(
        COPY ${OPENVR_DLL}
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()

# Copy OpenVR action bindings
file (
  COPY "${PROJECT_SOURCE_DIR}/actions.json"
  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Copy default bindings for gamepad
file (
  COPY "${PROJECT_SOURCE_DIR}/gamepad_bindings.json"
  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
