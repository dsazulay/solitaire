#include "window.h"

#include "dispatcher.h"
#include "event.h"
#include "utils/log.h"

double Window::xPos, Window::yPos;
float Window::lastClickTime = 0.0f;
float Window::dragStartTime = 0.0f;

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

void Window::createWindow(int width, int height, const char *name)
{
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

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::frameBufferCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    Window::xPos = xPos;
    // Invert y position so that 0 is on the bottom
    Window::yPos = 720 - yPos;
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        dragStartTime = (float) glfwGetTime();
        MouseDragStartEvent dragStartEvent(xPos, yPos);
        Dispatcher::instance().post(dragStartEvent);
    }

    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        MouseDragEndEvent dragEndEvent(xPos, yPos);
        Dispatcher::instance().post(dragEndEvent);

        float clickTime = (float) glfwGetTime();
        float timeDiff = clickTime - lastClickTime;
        lastClickTime = clickTime;

        if (timeDiff > 0.05 && timeDiff < 0.2)
        {
            MouseDoubleClickEvent doubleClickEvent(xPos, yPos);
            Dispatcher::instance().post(doubleClickEvent);
            return;
        }

        if (clickTime - dragStartTime < 0.05)
        {
            //MouseClickEvent e(xPos, yPos);
            //Dispatcher::instance().post(e);
        }
    }
}

void Window::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    
    else if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(0);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(1);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(2);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(3);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(4);
        Dispatcher::instance().post(e);
    }

    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        KeyboardPressEvent e(5);
        Dispatcher::instance().post(e);
    }
}


GLFWwindow* Window::getWindow()
{
    return m_window;
}
