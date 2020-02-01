cmake_minimum_required(VERSION 3.11)
project(stereo)

set(CMAKE_CXX_STANDARD 11)

option(BUILD_DOCUMENTATION "Bulid the documentation with Doxygen" ON)

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
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else (DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif (BUILD_DOCUMENTATION)


# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})


# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw)
set(GLFW_INCLUDE_DIR libs/glfw/include)


# GLAD
add_subdirectory(libs/glad-generated)
set_property(TARGET ${GLAD_LIBRARIES} PROPERTY POSITION_INDEPENDENT_CODE ON)

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
    src/stereo.cpp
)

set(STEREO_INCLUDES
    PUBLIC include
    PUBLIC ${GLAD_INCLUDE_DIRS}
)

set(STEREO_LIBRARIES
    ${GLAD_LIBRARIES}
)

add_library(stereo SHARED ${STEREO_SOURCES})
add_library(stereo_static STATIC ${STEREO_SOURCES})

target_include_directories(stereo ${STEREO_INCLUDES})
target_include_directories(stereo_static ${STEREO_INCLUDES})

target_link_libraries(stereo ${STEREO_LIBRARIES})
target_link_libraries(stereo_static ${STEREO_LIBRARIES})


# Stereo Executable
set(STEREO_BIN_SOURCES
    src/main.cpp
)
add_executable(stereo-world ${STEREO_BIN_SOURCES})
target_include_directories(stereo-world 
    PUBLIC include 
    PUBLIC ${GLFW_INCLUDE_DIR}
)
target_link_libraries(stereo-world glfw stereo_static)

