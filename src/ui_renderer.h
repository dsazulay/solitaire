#pragma once

#include <glad/glad.h>
#include "GLFW/glfw3.h"

class UiRenderer
{
public:
    UiRenderer(GLFWwindow* window);
    ~UiRenderer();
    void render();
    int renderMode();

private:
    int m_renderMode{};
};
