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
    void showDebugWindow();
    void hideDebugWindow();
    void showWonWindow();
    void hideWonWindow();

private:
    void renderWonWindow();
    void renderDebugWindow();

    int m_renderMode{};
    bool m_shouldRenderWonWindow{};
    bool m_shouldRenderDebugWindow{};
};
