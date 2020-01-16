cmake_minimum_required(VERSION 2.8)

include_directories(include)

project(stereo)
add_library(stereo SHARED src/lib.cpp)
add_executable(stereo-world src/main.cpp)

