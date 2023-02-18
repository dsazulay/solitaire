#include "solitaire.h"

#include <functional>
#include <thread>
#include <chrono>

#include "timer.h"
#include "keycodes.h"
#include "resource_manager.h"

Solitaire::Solitaire()
{
    m_appConfig.windowName = "Solitaire";
    m_appConfig.windowWidth = Solitaire::defaultWindowWidth;
    m_appConfig.windowHeight = Solitaire::defaultWindowHeight;
}

auto Solitaire::run() -> void
{
    init();
    mainLoop();
}

auto Solitaire::init() -> void
{
    m_window = std::make_unique<Window>();
    m_window->createWindow(m_appConfig.windowWidth, m_appConfig.windowHeight, m_appConfig.windowName.c_str());

    m_renderer = std::make_unique<Renderer>();

    m_uiRenderer = std::make_unique<UiRenderer>(m_window->getWindow());

    // Game init
    m_freecell.init();
    m_uiRenderer->setPlayerAndMatchData(m_freecell.playerData(), m_freecell.matchData());

    Dispatcher<MouseClickEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onMouseClick(arg); });
    Dispatcher<MouseDoubleClickEvent>::subscribe( 
        [&] (const auto& arg) { Solitaire::onMouseDoubleClick(arg); });
    Dispatcher<MouseDragEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onMouseDrag(arg); });
    Dispatcher<KeyboardPressEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onKeyboardPress(arg); });
    Dispatcher<GameWinEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onGameWin(arg); });
    Dispatcher<UiNewGameEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onUiNewGameEvent(arg); });
    Dispatcher<UiRecompileShaderEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onUiRecompileShaderEvent(arg); });

    //glfwSwapInterval(1); // Enable vsync
}

auto Solitaire::mainLoop() -> void
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

auto Solitaire::onMouseClick(const MouseClickEvent& e) -> void
{
    m_freecell.handleInputClick(e.xPos(), e.yPos(), false, false);
}

auto Solitaire::onMouseDoubleClick(const MouseDoubleClickEvent& e) -> void
{
    m_freecell.handleInputDoubleClick(e.xPos(), e.yPos());
}

auto Solitaire::onMouseDrag(const MouseDragEvent& e) -> void
{
    m_freecell.handleInputClick(e.xPos(), e.yPos(), true, e.isStart());
}

auto Solitaire::onKeyboardPress(const KeyboardPressEvent& e) -> void
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
    else if (e.key() == KeyCode::C)
    {
        ResourceManager::recompileShaders();
        m_renderer->reloadShaders();
    }
}

auto Solitaire::onGameWin(const GameWinEvent& e) -> void
{
    m_uiRenderer->showWonWindow();
}

auto Solitaire::onUiNewGameEvent(const UiNewGameEvent& e) -> void
{
    m_freecell.handleInputNewGame();
}

auto Solitaire::onUiRecompileShaderEvent(const UiRecompileShaderEvent& e) -> void
{
    ResourceManager::recompileShaders();
    m_renderer->reloadShaders();
}

