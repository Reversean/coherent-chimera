cmake_minimum_required(VERSION 3.26)
project(decoder)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(MPI REQUIRED)

add_executable(main main.cpp matrices.h
        matrices.cpp)

target_link_libraries(main PRIVATE MPI::MPI_CXX)
