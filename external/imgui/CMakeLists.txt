cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)
project(ImGui
        VERSION 1.0.0.0
        LANGUAGES C CXX
)


file(GLOB_RECURSE IMGUI_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)


add_library(
  ${PROJECT_NAME}
  "${IMGUI_SOURCES}"
)

target_link_libraries(
    ${PROJECT_NAME}
    glfw
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC external/glfw
)

add_dependencies(
    ${PROJECT_NAME}
    glfw
)


target_include_directories(
  ImGui
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
