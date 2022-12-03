#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();
    ~Window();
    void createWindow(int width, int height, const char* name);
    bool shouldClose() const;
    void processInput() const;
    void swapBuffers() const;
    void pollEvents();
    GLFWwindow* getWindow();

    static double xPos;
    static double yPos;

private:
    static void frameBufferCallback(GLFWwindow* window, int width, int height);
    static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    static float lastClickTime;
    static float dragStartTime;
    GLFWwindow* m_window;
};
