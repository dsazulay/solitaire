#include "solitaire.h"

#include <functional>
#include <thread>
#include <chrono>
#include <time.h>

#include "timer.h"

void Solitaire::onMouseClick(const Event& e)
{
    const auto& event = static_cast<const MouseClickEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.handleInputClick(xPos, yPos, false, false);
}

void Solitaire::onMouseDoubleClick(const Event& e)
{
    const auto& event = static_cast<const MouseDoubleClickEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.handleInputDoubleClick(xPos, yPos);
}


void Solitaire::onMouseDragStart(const Event& e)
{
    const auto& event = static_cast<const MouseDragStartEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.handleInputClick(xPos, yPos, true, true);
}


void Solitaire::onMouseDragEnd(const Event& e)
{
    const auto& event = static_cast<const MouseDragEndEvent&>(e);
    double xPos = event.xPos();
    double yPos = event.yPos();

    m_freecell.handleInputClick(xPos, yPos, true, false);
}

void Solitaire::onKeyboardPress(const Event& e)
{
    const auto& event = static_cast<const KeyboardPressEvent&>(e);
    int key = event.key();

    if (key == 0)
        m_freecell.handleInputUndo();
    else if (key == 1)
        m_freecell.handleInputRedo();
    else if (key == 2)
        m_freecell.handleInputRestart();
    else if (key == 3)
        m_freecell.handleInputNewGame();
    else if (key == 4)
        m_uiRenderer->toggleStatsWindow();
    else if (key == 5)
        m_uiRenderer->toggleDebugWindow();
}

void Solitaire::onGameWin(const Event& e)
{
    m_uiRenderer->showWonWindow();
}

void Solitaire::onUiNewGameEvent(const Event& e)
{
    m_freecell.handleInputNewGame();
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
}

void Solitaire::init()
{
    m_window = std::make_unique<Window>();
    m_window->createWindow(m_appConfig.windowWidth, m_appConfig.windowHeight, m_appConfig.windowName.c_str());

    m_renderer = std::make_unique<Renderer>();

    m_uiRenderer = std::make_unique<UiRenderer>(m_window->getWindow());

    // Game init
    srand(time(NULL));
    m_freecell.init();
    m_uiRenderer->setPlayerAndMatchData(m_freecell.playerData(), m_freecell.matchData());

    bindCallback(MouseClickEvent::descriptor, &Solitaire::onMouseClick);
    bindCallback(MouseDoubleClickEvent::descriptor, &Solitaire::onMouseDoubleClick);
    bindCallback(MouseDragStartEvent::descriptor, &Solitaire::onMouseDragStart);
    bindCallback(MouseDragEndEvent::descriptor, &Solitaire::onMouseDragEnd);
    bindCallback(KeyboardPressEvent::descriptor, &Solitaire::onKeyboardPress);
    bindCallback(GameWinEvent::descriptor, &Solitaire::onGameWin);
    bindCallback(UiNewGameEvent::descriptor, &Solitaire::onUiNewGameEvent);

    //glfwSwapInterval(1); // Enable vsync
}

void Solitaire::mainLoop()
{
    while (!m_window->shouldClose())
    {
        Timer::update();
        // TODO: change harcoded value for value selected by user
        auto target_fps = std::chrono::steady_clock::now() + std::chrono::milliseconds(14);

        m_freecell.update();
        m_renderer->render(m_freecell.board(), (RenderMode) m_uiRenderer->renderMode());
        m_uiRenderer->render();

        std::this_thread::sleep_until(target_fps);

        m_window->swapBuffers();
        m_window->pollEvents();
    }
}
