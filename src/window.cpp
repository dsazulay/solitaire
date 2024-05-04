#include "window.h"

#include <imgui.h>

#include "dispatcher.h"
#include "event.h"
#include "utils/log.h"
#include "keycodes.h"

#define UNUSED(x) (void)x;

constexpr const float doubleClickMinTime = 0.05f;
constexpr const float doubleClickMaxTime = 0.2f;
glm::vec2 Window::mousePos{};

Window::Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

Window::~Window()
{
    glfwTerminate();
}

auto Window::createWindow(int width, int height, const char *name) -> void
{
    m_windowSize = { static_cast<float>(width), static_cast<float>(height) };
    m_glfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (m_glfwWindow == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(m_glfwWindow);
    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwSetFramebufferSizeCallback(m_glfwWindow, frameBufferCallback);
    glfwSetCursorPosCallback(m_glfwWindow, cursorPositionCallback);
    glfwSetMouseButtonCallback(m_glfwWindow, mouseButtonCallback);
    glfwSetKeyCallback(m_glfwWindow, keyboardCallback);

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
            "Failed to initialize GLAD");
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

auto Window::cursorPositionCallback(double x, double y) -> void
{
    // Invert y position so that 0 is on the bottom
    mousePos = { static_cast<float>(x), m_windowSize.y - static_cast<float>(y) };
}

auto Window::mouseButtonCallback(int button, int action) -> void
{
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        m_dragStartTime = static_cast<float>(glfwGetTime());
        MouseDragEvent dragEvent(mousePos.x, mousePos.y, true);
        Dispatcher<MouseDragEvent>::post(dragEvent);
    }

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        MouseDragEvent dragEvent(mousePos.x, mousePos.y, false);
        Dispatcher<MouseDragEvent>::post(dragEvent);

        auto clickTime = (float) glfwGetTime();
        float timeDiff = clickTime - m_lastClickTime;
        m_lastClickTime = clickTime;

        if (timeDiff > doubleClickMinTime && timeDiff < doubleClickMaxTime)
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

auto Window::frameBufferCallback(GLFWwindow* window, int width,
        int height) -> void
{
    UNUSED(window);
    glViewport(0, 0, width, height);
}

auto Window::cursorPositionCallback(GLFWwindow* window, double xpos,
        double ypos) -> void
{
    auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));
    w->cursorPositionCallback(xpos, ypos);
}

auto Window::mouseButtonCallback(GLFWwindow* window, int button, int action,
        int mods) -> void
{
    UNUSED(mods);
    auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));
    w->mouseButtonCallback(button, action);
}

auto Window::keyboardCallback(GLFWwindow* window, int key, int scancode,
        int action, int mods) -> void
{
    UNUSED(scancode);
    UNUSED(mods);
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

auto Window::getGlfwWindow() -> GLFWwindow*
{
    return m_glfwWindow;
}
