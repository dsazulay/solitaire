#include "solitaire.h"

#include "resource_manager.h"
#include "shader.h"
#include "utils/log.h"

float Solitaire::deltaTime;

Solitaire::Solitaire()
{
    m_appConfig.windowName = "Solitaire";
    m_appConfig.windowWidth = 1280;
    m_appConfig.windowHeight = 720;
}

void Solitaire::run()
{
    init();
    mainLoop();
    terminate();
}

void Solitaire::init()
{
    m_window = new Window();
    m_window->init();
    m_window->createWindow(m_appConfig.windowWidth, m_appConfig.windowHeight, m_appConfig.windowName.c_str());

    ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");

    m_renderer = new Renderer();
    m_renderer->init();
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        calculateDeltaTime();

        m_window->processInput();

        m_renderer->render();

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}

void Solitaire::terminate()
{
    m_renderer->terminate();
    m_window->terminate();

    delete m_renderer;
    delete m_window;
}

void Solitaire::calculateDeltaTime()
{
    float currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
}
