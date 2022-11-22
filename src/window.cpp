#include "window.h"

#include "GLFW/glfw3.h"
#include "dispatcher.h"
#include "event.h"
#include "utils/log.h"

double Window::xPos, Window::yPos;
float Window::lastClickTime = 0.0f;
float Window::dragStartTime = 0.0f;

void Window::init()
{
  glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Window::createWindow(int width, int height, const char *name)
{
    m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (m_window == nullptr)
    {
        LOG_ERROR("Failed to create GLFW window");
        terminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, frameBufferCallback);
    glfwSetCursorPosCallback(m_window, cursorPositionCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
}

void Window::terminate()
{
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::processInput() const
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
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
        MouseDragStartEvent dragEndEvent(xPos, yPos);
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

GLFWwindow* Window::getWindow()
{
    return m_window;
}
