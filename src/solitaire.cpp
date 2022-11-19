#include "solitaire.h"

#include <glm/vec2.hpp>

#include <vector>
#include <time.h>

#include "dispatcher.h"
#include "resource_manager.h"
#include "shader.h"
#include "timer.h"
#include "ui_renderer.h"
#include "utils/log.h"


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


    m_uiRenderer = std::make_unique<UiRenderer>(m_window->getWindow());

    // Game init
    srand(time(NULL));
    m_freecell.init();

    Dispatcher::instance().subscribe(MouseClickEvent::descriptor,
        std::bind(&Solitaire::onMouseClick, this, std::placeholders::_1));


    //glfwSwapInterval(1); // Enable vsync
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        Timer::update();

        m_window->processInput();

        m_renderer->render(m_freecell.m_map, m_freecell.m_table);
        m_renderer->renderOpenCellsAndFoundation(m_freecell.m_openCellsMap, m_freecell.m_openCells);
        m_renderer->renderOpenCellsAndFoundation(m_freecell.m_foundationMap, m_freecell.m_foundations);
        m_renderer->drawCall();
        m_uiRenderer->render();


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
