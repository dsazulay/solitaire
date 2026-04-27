#include "window.h"

#include "dispatcher.h"
#include "event.h"
#include "glm/ext/vector_int2.hpp"
#include "keycodes.h"
#include "utils/log.h"
#include <vulkan/vulkan_core.h>

constexpr f32 DOUBLE_CLICK_MIN_TIME = 0.05f;
constexpr f32 DOUBLE_CLICK_MAX_TIME = 0.2f;
glm::vec2 Window::mousePos{};

auto Window::init() -> void
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

auto Window::terminate() -> void
{
    glfwTerminate();
}

auto Window::createWindow(i32 width, i32 height, const char *name) -> void
{
    m_windowSize = { width, height };
    m_glfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (m_glfwWindow == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(m_glfwWindow);
    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwSetFramebufferSizeCallback(m_glfwWindow, frameBufferSizeCallback);
    glfwSetCursorPosCallback(m_glfwWindow, cursorPositionCallback);
    glfwSetMouseButtonCallback(m_glfwWindow, mouseButtonCallback);
    glfwSetKeyCallback(m_glfwWindow, keyboardCallback);
}

auto Window::shouldClose() const -> bool
{
    return glfwWindowShouldClose(m_glfwWindow);
}

auto Window::isFocused() const -> bool
{
    return glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED);
}

auto Window::swapBuffers() const -> void
{
    glfwSwapBuffers(m_glfwWindow);
}

auto Window::pollEvents() -> void
{
    glfwPollEvents();
}

auto Window::createVulkanSurface(VkInstance instance) -> VkSurfaceKHR
{
    VkSurfaceKHR surface;
    // TODO: add check
    glfwCreateWindowSurface(instance, m_glfwWindow, nullptr, &surface);

    return surface;
}

auto Window::getRequiredExtensions(u32 *extensionsCount) -> const char**
{
    return glfwGetRequiredInstanceExtensions(extensionsCount);
}

auto Window::getFramebufferSize() -> glm::ivec2
{
    glm::ivec2 size;
    glfwGetFramebufferSize(m_glfwWindow, &size.x, &size.y);
    return size;
}

auto Window::getGlfwWindow() -> GLFWwindow*
{
    return m_glfwWindow;
}

auto Window::cursorPositionCallback(f64 x, f64 y) -> void
{
    // Invert y position so that 0 is on the bottom
    mousePos = { (f32) x, ((f32) m_windowSize.y) - (f32) y };
}

auto Window::mouseButtonCallback(i32 button, i32 action) -> void
{
    // TODO: move this to imgui class
    //auto& io = ImGui::GetIO();
    //if (io.WantCaptureMouse)
    //    return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        m_dragStartTime = (f32) glfwGetTime();
        MouseDragEvent dragEvent(mousePos.x, mousePos.y, true);
        Dispatcher<MouseDragEvent>::post(dragEvent);
    }

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        MouseDragEvent dragEvent(mousePos.x, mousePos.y, false);
        Dispatcher<MouseDragEvent>::post(dragEvent);

        auto clickTime = (f32) glfwGetTime();
        float timeDiff = clickTime - m_lastClickTime;
        m_lastClickTime = clickTime;

        if (timeDiff > DOUBLE_CLICK_MIN_TIME && timeDiff < DOUBLE_CLICK_MAX_TIME)
        {
            MouseDoubleClickEvent doubleClickEvent(mousePos.x, mousePos.y);
            Dispatcher<MouseDoubleClickEvent>::post(doubleClickEvent);
            return;
        }

        //if (clickTime - m_dragStartTime < 0.05)
        //{
        //    MouseClickEvent e(mousePos.x, mousePos.y);
        //    Dispatcher<MouseClickEvent>::post(e);
        //}
    }
}

auto Window::frameBufferSizeCallback(GLFWwindow*, i32, i32) -> void
{
    // TODO: afjust the surface size for vulakn
}

auto Window::cursorPositionCallback(GLFWwindow* window, f64 xpos, f64 ypos) -> void
{
    Window* w = (Window*)glfwGetWindowUserPointer(window);
    w->cursorPositionCallback(xpos, ypos);
}

auto Window::mouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32) -> void
{
    Window* w = (Window*)glfwGetWindowUserPointer(window);
    w->mouseButtonCallback(button, action);
}

auto Window::keyboardCallback(GLFWwindow* window, i32 key, i32, i32 action, i32) -> void
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    else if (action == GLFW_PRESS)
    {
        KeyboardPressEvent e((KeyCode) key);
        Dispatcher<KeyboardPressEvent>::post(e);
    }
}

