cmake_minimum_required(VERSION 3.26)
project(matrix-rank)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(default-thread-count default-thread-count.cpp matrix-rank.cpp)
add_executable(paramertized-thread-count paramertized-thread-count.cpp matrix-rank.cpp)
