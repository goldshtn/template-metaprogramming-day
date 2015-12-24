cmake_minimum_required(VERSION 3.3)
project(TMP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp variadics.h compile_time_computation.h common.h traits.h member_detection.h sequences.h policies.h tuple_cat.h solutions.h)
add_executable(TMP ${SOURCE_FILES})