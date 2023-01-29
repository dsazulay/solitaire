#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class Window
{
public:
    Window();
    Window(const Window& w) = delete;
    Window(Window&& w) = delete;
    auto operator=(const Window& w) -> Window& = delete;
    auto operator=(Window&& w) -> Window& = delete;
    ~Window();

    auto createWindow(int width, int height, const char* name) -> void;
    [[nodiscard]] auto shouldClose() const -> bool;
    auto swapBuffers() const -> void;
    auto pollEvents() -> void;
    auto getWindow() -> GLFWwindow*;

    static glm::vec2 mousePos;

private:
    static auto frameBufferCallback(GLFWwindow* window, int width, int height) -> void;
    static auto cursorPositionCallback(GLFWwindow* window, double xPos, double yPos) -> void;
    static auto mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) -> void;
    static auto keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void;

    static float lastClickTime;
    static float dragStartTime;
    static glm::vec2 windowSize;

    GLFWwindow* m_window{};
};
