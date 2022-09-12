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

private:
    static void frameBufferCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window;
};
