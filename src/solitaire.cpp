#include "solitaire.h"

#include <functional>
#include <thread>
#include <chrono>
#include <ctime>

#include "timer.h"
#include "keycodes.h"

void Solitaire::onMouseClick(const MouseClickEvent& e)
{
    m_freecell.handleInputClick(e.xPos(), e.yPos(), false, false);
}

void Solitaire::onMouseDoubleClick(const MouseDoubleClickEvent& e)
{
    m_freecell.handleInputDoubleClick(e.xPos(), e.yPos());
}

void Solitaire::onMouseDragStart(const MouseDragStartEvent& e)
{
    m_freecell.handleInputClick(e.xPos(), e.yPos(), true, true);
}

void Solitaire::onMouseDragEnd(const MouseDragEndEvent& e)
{
    m_freecell.handleInputClick(e.xPos(), e.yPos(), true, false);
}

void Solitaire::onKeyboardPress(const KeyboardPressEvent& e)
{
    if (e.key() == KeyCode::U)
        m_freecell.handleInputUndo();
    else if (e.key() == KeyCode::R)
        m_freecell.handleInputRedo();
    else if (e.key() == KeyCode::E)
        m_freecell.handleInputRestart();
    else if (e.key() == KeyCode::N)
        m_freecell.handleInputNewGame();
    else if (e.key() == KeyCode::S)
        m_uiRenderer->toggleStatsWindow();
    else if (e.key() == KeyCode::D)
        m_uiRenderer->toggleDebugWindow();
}

void Solitaire::onGameWin(const GameWinEvent& e)
{
    m_uiRenderer->showWonWindow();
}

void Solitaire::onUiNewGameEvent(const UiNewGameEvent& e)
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
    srand(time(nullptr));
    m_freecell.init();
    m_uiRenderer->setPlayerAndMatchData(m_freecell.playerData(), m_freecell.matchData());

    Dispatcher<MouseClickEvent>::instance().subscribe(
        [&] (const auto& arg) { Solitaire::onMouseClick(arg); });
    Dispatcher<MouseDoubleClickEvent>::instance().subscribe( 
        [&] (const auto& arg) { Solitaire::onMouseDoubleClick(arg); });
    Dispatcher<MouseDragStartEvent>::instance().subscribe(
        [&] (const auto& arg) { Solitaire::onMouseDragStart(arg); });
    Dispatcher<MouseDragEndEvent>::instance().subscribe(
        [&] (const auto& arg) { Solitaire::onMouseDragEnd(arg); });
    Dispatcher<KeyboardPressEvent>::instance().subscribe(
        [&] (const auto& arg) { Solitaire::onKeyboardPress(arg); });
    Dispatcher<GameWinEvent>::instance().subscribe(
        [&] (const auto& arg) { Solitaire::onGameWin(arg); });
    Dispatcher<UiNewGameEvent>::instance().subscribe(
        [&] (const auto& arg) { Solitaire::onUiNewGameEvent(arg); });

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
