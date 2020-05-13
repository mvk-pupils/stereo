cmake_minimum_required(VERSION 3.10)

set(PROJECT_ROOT ${PROJECT_SOURCE_DIR})

project(stereo-world)

find_package(GLFW3)

# GLFW
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# add_subdirectory(${PROJECT_ROOT}/libs/glfw glfw)
# set(GLFW_INCLUDE_DIR ${PROJECT_ROOT}/libs/glfw/include)


# Iconic API (proprietary)
set(ICONIC_INCLUDE_DIR "$ENV{ICONICPATH}/IconicGpu/include")
set(ICONIC_LIBRARIES
    "$ENV{ICONICPATH}/IconicGpu/lib/x64/Release/IconicVideo.lib"
    "$ENV{ICONICPATH}/IconicGpu/lib/x64/Release/IconicMatch.lib"
    "$ENV{ICONICPATH}/IconicGpu/lib/x64/Release/IconicGpu.lib"
)
set(ICONIC_INCLUDE_DIR "$ENV{ICONICPATH}/IconicGpu/include")

file(GLOB ICONIC_DLL_LIST "$ENV{ICONICPATH}/IconicGpu/x64/Release/*.dll")
message("DLLS: ${ICONIC_DLL_LIST}")
foreach(ICONIC_DLL ${ICONIC_DLL_LIST})
    message("DLL: ${ICONIC_DLL}")
    file(
        COPY "${ICONIC_DLL}"
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endforeach(ICONIC_DLL)

find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)

# Stereo Executable
set(STEREO_BIN_SOURCES
    src/main.cpp
    src/cli.cpp
    src/window.cpp
)

message(STATUS "Eigen ${EIGEN3_INCLUDE_DIR}")

set(STEREO_BIN_INCLUDES
    PUBLIC include
    PUBLIC ${STEREO_INCLUDE_DIR}
    PUBLIC ${GLFW_INCLUDE_DIR}
    PUBLIC ${ICONIC_INCLUDE_DIR}
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${EIGEN3_INCLUDE_DIR}
)

set(STEREO_BIN_LIBRARIES
    stereo-static
    glfw
    ${Boost_LIBRARIES}
    ${ICONIC_LIBRARIES}
)

add_executable(stereo-world EXCLUDE_FROM_ALL ${STEREO_BIN_SOURCES})
target_include_directories(stereo-world ${STEREO_BIN_INCLUDES})
target_link_libraries(stereo-world ${STEREO_BIN_LIBRARIES})

# Set the prefix to use when printing logging information
target_compile_definitions(stereo-world PRIVATE LOG_SOURCE_MODULE="stereo-world")
