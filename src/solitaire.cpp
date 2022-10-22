#include "solitaire.h"

#include "dispatcher.h"
#include "resource_manager.h"
#include "shader.h"
#include "utils/log.h"

#include <vector>
#include <glm/vec2.hpp>

#include <time.h>

float Solitaire::deltaTime;

void Solitaire::onMouseClick(const Event& e)
{
    const auto& event = static_cast<const MouseClickEvent&>(e);
    double xpos = event.xpos();
    double ypos = event.ypos();

    m_freecell.processInput(xpos, ypos);
}

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

    // Game init
    srand(time(NULL));
    m_freecell.init();

    Dispatcher::instance().subscribe(MouseClickEvent::descriptor,
        std::bind(&Solitaire::onMouseClick, this, std::placeholders::_1));
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        calculateDeltaTime();

        m_window->processInput();

        m_renderer->render(m_freecell.m_map, m_freecell.m_table);
        m_renderer->renderOpenCellsAndFoundation(m_freecell.m_openCellsMap, m_freecell.m_openCells);
        m_renderer->renderOpenCellsAndFoundation(m_freecell.m_foundationMap, m_freecell.m_foundations);

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
