# Project Info

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
include(FetchContent)

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

# SDL3, Volk, VMA, glm and Slang are taken from the SDK
include_directories(SYSTEM $ENV{VULKAN_SDK}/include)
include_directories(external/imgui)
include_directories(external/ktx/include)
include_directories(external/ktx/other_include)

# glfw
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS NAMES glfw3
)
FetchContent_MakeAvailable(glfw)

# glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(glm)

# fmt 
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(fmt)

# ktx
set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/ktx)
set(KTX_SOURCES
    ${KTX_DIR}/lib/texture.c
    ${KTX_DIR}/lib/hashlist.c
    ${KTX_DIR}/lib/checkheader.c
    ${KTX_DIR}/lib/swap.c
    ${KTX_DIR}/lib/memstream.c
    ${KTX_DIR}/lib/filestream.c
    ${KTX_DIR}/lib/vkloader.c)
add_library(ktx STATIC ${KTX_SOURCES})

find_library(Slang_LIBRARY NAMES slang slang-compiler HINTS "$ENV{VULKAN_SDK}/lib" REQUIRED)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_vulkan.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})

target_compile_definitions(imgui PRIVATE IMGUI_IMPL_VULKAN_NO_PROTOTYPES)

target_link_libraries(imgui glfw)

# stb_image
message(STATUS "Installing stb_image")
add_subdirectory(external/stb_image)
set_property(TARGET stb_image PROPERTY FOLDER "Dependencies")

# tiny_obj_loader
message(STATUS "Installing tiny_obj_loader")
add_subdirectory(external/tiny_obj_loader)
set_property(TARGET tiny_obj_loader PROPERTY FOLDER "Dependencies")

# doctest
message(STATUS "Installing doctest")
add_subdirectory(external/doctest)
set_property(TARGET doctest PROPERTY FOLDER "Dependencies")

# =============================================================

# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

# =============================================================

# Finalize App

add_executable(Solitaire "${FILE_SOURCES}")

target_compile_definitions(Solitaire PRIVATE VK_NO_PROTOTYPES)
target_compile_definitions(Solitaire PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

# =============================================================

# Finish Dependencies

target_link_libraries(Solitaire glfw glm fmt imgui stb_image
    tiny_obj_loader doctest ktx ${Slang_LIBRARY})

# =============================================================

# Finish Settings

target_compile_options(Solitaire PRIVATE -Wall -Wextra -Wshadow
                       -Wnon-virtual-dtor -pedantic)

# Change output dir to bin
set_target_properties(Solitaire PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
