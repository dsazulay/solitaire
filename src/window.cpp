#include "window.h"

#include <imgui.h>

#include "dispatcher.h"
#include "event.h"
#include "utils/log.h"
#include "keycodes.h"

float Window::lastClickTime = 0.0f;
float Window::dragStartTime = 0.0f;
glm::vec2 Window::mousePos{};
glm::vec2 Window::windowSize{};

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
    windowSize.x = (float) width;
    windowSize.y = (float) height;
    m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (m_window == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, frameBufferCallback);
    glfwSetCursorPosCallback(m_window, cursorPositionCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetKeyCallback(m_window, keyboardCallback);

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");
}

auto Window::shouldClose() const -> bool
{
    return glfwWindowShouldClose(m_window);
}

auto Window::swapBuffers() const -> void
{
    glfwSwapBuffers(m_window);
}

auto Window::pollEvents() -> void
{
    glfwPollEvents();
}

auto Window::frameBufferCallback(GLFWwindow* window, int width, int height) -> void
{
    glViewport(0, 0, width, height);
}

auto Window::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos) -> void
{
    mousePos.x = (float) xPos;
    // Invert y position so that 0 is on the bottom
    mousePos.y = windowSize.y - (float) yPos;
}

auto Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) -> void
{
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        dragStartTime = (float) glfwGetTime();
        MouseDragEvent dragEvent(mousePos.x, mousePos.y, true);
        Dispatcher<MouseDragEvent>::post(dragEvent);
    }

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        MouseDragEvent dragEvent(mousePos.x, mousePos.y, false);
        Dispatcher<MouseDragEvent>::post(dragEvent);

        auto clickTime = (float) glfwGetTime();
        float timeDiff = clickTime - lastClickTime;
        lastClickTime = clickTime;

        const float doubleClickMinTime = 0.05f;
        const float doubleClickMaxTime = 0.2f;
        if (timeDiff > doubleClickMinTime && timeDiff < doubleClickMaxTime)
        {
            MouseDoubleClickEvent doubleClickEvent(mousePos.x, mousePos.y);
            Dispatcher<MouseDoubleClickEvent>::post(doubleClickEvent);
            return;
        }

        // if (clickTime - dragStartTime < 0.05)
        // {
            //MouseClickEvent e(mousePos.x, mousePos.y);
            //Dispatcher::instance().post(e);
        // }
    }
}

auto Window::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
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

auto Window::getWindow() -> GLFWwindow*
{
    return m_window;
}
