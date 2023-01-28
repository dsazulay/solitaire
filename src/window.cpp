#include "window.h"

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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        dragStartTime = (float) glfwGetTime();
        MouseDragStartEvent dragStartEvent(mousePos.x, mousePos.y);
        Dispatcher::instance().post(dragStartEvent);
    }

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        MouseDragEndEvent dragEndEvent(mousePos.x, mousePos.y);
        Dispatcher::instance().post(dragEndEvent);

        auto clickTime = (float) glfwGetTime();
        float timeDiff = clickTime - lastClickTime;
        lastClickTime = clickTime;

        const float doubleClickMinTime = 0.05f;
        const float doubleClickMaxTime = 0.2f;
        if (timeDiff > doubleClickMinTime && timeDiff < doubleClickMaxTime)
        {
            MouseDoubleClickEvent doubleClickEvent(mousePos.x, mousePos.y);
            Dispatcher::instance().post(doubleClickEvent);
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
    
    else if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(KeyCode::U);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(KeyCode::R);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(KeyCode::E);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(KeyCode::N);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(KeyCode::S);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(KeyCode::D);
        Dispatcher::instance().post(e);
    }
}

auto Window::getWindow() -> GLFWwindow*
{
    return m_window;
}
