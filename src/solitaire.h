#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

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

    AppConfig m_appConfig;
    GLFWwindow* m_window;
};
