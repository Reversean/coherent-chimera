cmake_minimum_required(VERSION 3.26)
project(primes)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(primes main.cpp
        miller-rabin-test.h
        miller-rabin-test.cpp)
