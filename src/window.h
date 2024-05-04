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
    [[nodiscard]] auto isFocused() const -> bool;
    auto swapBuffers() const -> void;
    auto pollEvents() -> void;
    auto getGlfwWindow() -> GLFWwindow*;

    static glm::vec2 mousePos;

private:
    auto cursorPositionCallback(double x, double y) -> void;
    auto mouseButtonCallback(int button, int action) -> void;

    static auto frameBufferCallback(GLFWwindow* window, int width,
            int height) -> void;
    static auto cursorPositionCallback(GLFWwindow* window, double xpos,
            double ypos) -> void;
    static auto mouseButtonCallback(GLFWwindow* window, int button, int action,
            int mods) -> void;
    static auto keyboardCallback(GLFWwindow* window, int key, int scancode,
            int action, int mods) -> void;

    float m_lastClickTime{};
    float m_dragStartTime{};
    glm::vec2 m_windowSize{};
    GLFWwindow* m_glfwWindow{};
};
