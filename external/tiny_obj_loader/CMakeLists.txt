cmake_minimum_required(VERSION 3.20)
project(tiny_obj_loader VERSION 1.0.0.0 LANGUAGES C CXX)

add_library(tiny_obj_loader INTERFACE)

target_include_directories(tiny_obj_loader INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

