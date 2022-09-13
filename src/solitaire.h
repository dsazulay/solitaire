#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "window.h"
#include "renderer.h"

struct AppConfig
{
    std::string windowName;
    uint32_t windowWidth;
    uint32_t windowHeight;
};

class Solitaire
{
public:
    Solitaire();
    void run();

    static float deltaTime;
private:
    void init();
    void mainLoop();
    void terminate();
    void calculateDeltaTime();

    AppConfig m_appConfig;
    Window* m_window;
    Renderer* m_renderer;
    float m_lastFrame;
};
