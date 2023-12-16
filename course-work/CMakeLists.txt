cmake_minimum_required(VERSION 3.26)
project(decoder)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(main main.cpp
        matrix.cpp
        matrix.h)
add_executable(generate-matrix generate-matrix.cpp)
