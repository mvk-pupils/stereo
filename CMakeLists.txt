cmake_minimum_required(VERSION 3.11)
project(stereo)

set(CMAKE_CXX_STANDARD 11)


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


# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})


# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)
set(GLFW_INCLUDE_DIR libs/glfw/include)


# OpenVR
if(WIN32)
  set(WINDOWS_PATH_SUFFIXES win${PLATFORM} Win${PLATFORM} x${PLATFORM})
endif()

find_library(OPENVR_LIBRARY
    NAMES
        openvr_api
    PATHS
        libs/openvr/bin
        libs/openvr/lib
    PATH_SUFFIXES
        osx32
        linux64
        ${WINDOWS_PATH_SUFFIXES}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)
set(OPENVR_INCLUDE_DIR libs/openvr/headers)


# Stereo Library
set(STEREO_SOURCES
    src/lib.cpp
)

set(STEREO_INCLUDES
    PUBLIC include
    PUBLIC ${GLFW_INCLUDE_DIR}
    PUBLIC ${OPENVR_INCLUDE_DIR}
)

add_library(stereo SHARED ${STEREO_SOURCES})
add_library(stereo_static STATIC ${STEREO_SOURCES})

target_include_directories(stereo ${STEREO_INCLUDES})
target_include_directories(stereo_static ${STEREO_INCLUDES})

target_link_libraries(stereo glfw ${OPENVR_LIBRARY})
target_link_libraries(stereo_static glfw ${OPENVR_LIBRARY})


# Stereo Executable
set(STEREO_BIN_SOURCES
    src/main.cpp
)
add_executable(stereo-world ${STEREO_BIN_SOURCES})
target_include_directories(stereo-world 
    PUBLIC include 
)
target_link_libraries(stereo-world stereo_static)

