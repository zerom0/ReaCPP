cmake_minimum_required(VERSION 3.5)
project(ReaCPP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(include)

add_subdirectory(examples)