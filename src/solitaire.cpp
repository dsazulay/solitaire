#include "solitaire.h"

#include <glm/vec2.hpp>

#include <functional>
#include <vector>
#include <thread>
#include <chrono>
#include <time.h>

#include "dispatcher.h"
#include "event.h"
#include "freecell.h"
#include "renderer.h"
#include "resource_manager.h"
#include "shader.h"
#include "timer.h"
#include "ui_renderer.h"
#include "utils/log.h"

void Solitaire::onMouseClick(const Event& e)
{
    const auto& event = static_cast<const MouseClickEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.processInput(xPos, yPos, false, false);
}

void Solitaire::onMouseDoubleClick(const Event& e)
{
    const auto& event = static_cast<const MouseDoubleClickEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.processDoubleClick(xPos, yPos);
}


void Solitaire::onMouseDragStart(const Event& e)
{
    const auto& event = static_cast<const MouseDragStartEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.processInput(xPos, yPos, true, true);
}


void Solitaire::onMouseDragEnd(const Event& e)
{
    const auto& event = static_cast<const MouseDragEndEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.processInput(xPos, yPos, true, false);
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
    Dispatcher::instance().subscribe(MouseDoubleClickEvent::descriptor,
        std::bind(&Solitaire::onMouseDoubleClick, this, std::placeholders::_1));
    Dispatcher::instance().subscribe(MouseDragStartEvent::descriptor,
        std::bind(&Solitaire::onMouseDragStart, this, std::placeholders::_1));
    Dispatcher::instance().subscribe(MouseDragEndEvent::descriptor,
        std::bind(&Solitaire::onMouseDragEnd, this, std::placeholders::_1));

    //glfwSwapInterval(1); // Enable vsync
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        Timer::update();
        // TODO: change harcoded value for value selected by user
        auto target_fps = std::chrono::steady_clock::now() + std::chrono::milliseconds(14);

        m_window->processInput();

        m_renderer->render(m_freecell.board(), (RenderMode) m_uiRenderer->renderMode());
        m_uiRenderer->render();

        std::this_thread::sleep_until(target_fps);

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}

void Solitaire::terminate()
{
    m_window->terminate();

    delete m_renderer;
    delete m_window;
}
