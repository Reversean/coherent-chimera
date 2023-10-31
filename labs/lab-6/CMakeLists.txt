cmake_minimum_required(VERSION 3.26)
project(decoder)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(MPI REQUIRED)

add_executable(point-to-point point-to-point.cpp)
add_executable(scatter scatter.cpp)

target_link_libraries(point-to-point PRIVATE MPI::MPI_CXX)
target_link_libraries(scatter PRIVATE MPI::MPI_CXX)
