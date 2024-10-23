cmake_minimum_required(VERSION 3.30)

project(figures)

set(CMAKE_BUILD_TYPE Debug)
add_executable(figures-test src/Figures-test.cpp)
target_include_directories(figures-test PUBLIC src)

