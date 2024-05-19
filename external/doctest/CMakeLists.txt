cmake_minimum_required(VERSION 3.20)
project(doctest VERSION 1.0.0.0 LANGUAGES C CXX)

add_library(doctest doctest.cpp)

target_include_directories(doctest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

