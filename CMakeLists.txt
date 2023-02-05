# Project Info

cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
cmake_policy(VERSION 3.6)
project(Solitaire
        VERSION 1.0.0.0
        LANGUAGES C CXX
)

# =============================================================

# CMake Settings

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SUPPRESS_REGENERATION true)
set(DCMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

# =============================================================

# Dependencies

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE)
message(STATUS "Installing glfw via submodule")
add_subdirectory(external/glfw)
set_property(TARGET glfw PROPERTY FOLDER "Dependencies")

# Glad
message(STATUS "Installing glad via submodule")
add_subdirectory(external/glad)
set_property(TARGET Glad PROPERTY FOLDER "Dependencies")

# glm
message(STATUS "Installing glm via sybmodule")
set(BUILD_STATIC_LIBS ON)
add_subdirectory(external/glm)
set_property(TARGET glm_static PROPERTY FOLDER "Dependencies")

# ImGui
message(STATUS "Installing imgui via submodule")
add_subdirectory(external/imgui)
set_property(TARGET ImGui PROPERTY FOLDER "Dependencies")

# fmt
message(STATUS "Installing fmt via submodule")
add_subdirectory(external/fmt)
set_property(TARGET fmt PROPERTY FOLDER "Dependencies")


# =============================================================

# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

# Solution Filters
foreach(source IN LISTS FILE_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    string(REPLACE "src" "" source_path_final "${source_path_msvc}")
    source_group("${source_path_final}" FILES "${source}")
endforeach()

# =============================================================

# Finalize App

add_executable(
    ${PROJECT_NAME}
    "${FILE_SOURCES}"
)

# =============================================================

# Finish Dependencies

target_link_libraries(
    ${PROJECT_NAME}
    Glad
    ImGui
    glfw
    glm_static
    fmt
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC external/glfw
  PUBLIC external/glm
  PUBLIC external/fmt
)

add_dependencies(
    ${PROJECT_NAME}
    glfw
    glm_static
    fmt
)

# =============================================================

# Finish Settings

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# Change output dir to bin
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
