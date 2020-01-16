cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 11)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)


# Stereo Library
project(stereo)
add_library(stereo SHARED src/lib.cpp)
target_include_directories(stereo 
    PUBLIC include 
    PUBLIC libs/glfw/include
)
target_link_libraries(stereo glfw)

# Stereo Executable
project(stereo-world)
add_executable(stereo-world src/main.cpp)
target_include_directories(stereo-world 
    PUBLIC include 
)
target_link_libraries(stereo-world stereo)

