#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
    void init();
    void createWindow(int width, int height, const char* name);
    void terminate();
    bool shouldClose() const;
    void processInput() const;
    void swapBuffers() const;
    void pollEvents();
    GLFWwindow* getWindow();

    static double xpos;
    static double ypos;

private:
    static void frameBufferCallback(GLFWwindow* window, int width, int height);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    GLFWwindow* m_window;
};
