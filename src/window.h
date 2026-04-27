#pragma once

#include "utils/types.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>

class Window
{
public:
    auto init() -> void;
    auto terminate() -> void;
    auto createWindow(int width, int height, const char* name) -> void;
    auto shouldClose() const -> bool;
    auto isFocused() const -> bool;
    auto swapBuffers() const -> void;
    auto pollEvents() -> void;
    auto createVulkanSurface(VkInstance instance) -> VkSurfaceKHR;
    auto getRequiredExtensions(u32* extensionsCount) -> const char**;
    auto getFramebufferSize() -> glm::ivec2;
    auto getGlfwWindow() -> GLFWwindow*;

    static glm::vec2 mousePos;

private:
    auto cursorPositionCallback(f64 x, f64 y) -> void;
    auto mouseButtonCallback(i32 button, i32 action) -> void;

    static auto frameBufferSizeCallback(
        GLFWwindow* window, i32 width, i32 height) -> void;
    static auto cursorPositionCallback(
        GLFWwindow* window, f64 xpos, f64 ypos) -> void;
    static auto mouseButtonCallback(
        GLFWwindow* window, i32 button, i32 action, int mods) -> void;
    static auto keyboardCallback(
        GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) -> void;

    f32 m_lastClickTime{};
    f32 m_dragStartTime{};
    glm::ivec2 m_windowSize{};
    GLFWwindow* m_glfwWindow{};
};
