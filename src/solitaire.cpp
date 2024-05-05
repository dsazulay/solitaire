#include "solitaire.h"

#include <thread>
#include <chrono>

#include "timer.h"
#include "keycodes.h"
#include "graphics/resource_manager.h"
#include "dispatcher.h"

Solitaire::Solitaire()
{
    constexpr const int defaultWindowWidth = 1280;
    constexpr const int defaultWindowHeight = 720;
    constexpr const double fps = 60.0;
    constexpr const double idleFps = 5.0;

    m_appConfig.windowName = "Solitaire";
    m_appConfig.windowWidth = defaultWindowWidth;
    m_appConfig.windowHeight = defaultWindowHeight;
    m_appConfig.fps = fps;
    m_appConfig.idleFps = idleFps;
}

auto Solitaire::run() -> void
{
    init();
    mainLoop();
}

auto Solitaire::init() -> void
{
    m_window.createWindow(m_appConfig.windowWidth,
            m_appConfig.windowHeight, m_appConfig.windowName.c_str());
    m_renderer.init();
    m_uiRenderer = std::make_unique<UiRenderer>(m_window.getGlfwWindow());

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
    Dispatcher<UiGameEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onUiGameEvent(arg); });
    Dispatcher<UiRecompileShaderEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onUiRecompileShaderEvent(arg); });
    Dispatcher<UiPrintCardEvent>::subscribe(
        [&] (const auto& arg) { Solitaire::onUiPrintCardEvent(arg); });

    // VSync not working on macos ventura
    //glfwSwapInterval(1); // Enable vsync
}

auto Solitaire::mainLoop() -> void
{
    const double frameMs = 1000.0 / m_appConfig.fps;
    const double frameIdleMs = 1000.0 / m_appConfig.idleFps;
    const std::chrono::duration<double, std::milli> frameTime(frameMs);
    const std::chrono::duration<double, std::milli> idleFrameTime(frameIdleMs);

    while (!m_window.shouldClose())
    {
        auto startTime = std::chrono::steady_clock::now();
        auto targetFps = startTime + frameTime;
        if (!m_window.isFocused())
        {
            targetFps = startTime + idleFrameTime;

            Timer::halt();
            m_window.pollEvents();

            std::this_thread::sleep_until(targetFps);

            continue;
        }
        Timer::update();

        m_freecell.update();
        m_renderer.render(m_freecell.board().cards, m_freecell.board().openCellsAndFoundBg,
                (RenderMode) m_uiRenderer->renderMode());
        m_uiRenderer->render();

        m_window.swapBuffers();
        m_window.pollEvents();

        std::this_thread::sleep_until(targetFps);
    }
}

auto Solitaire::sleepToTargetFps(std::chrono::time_point<std::chrono::steady_clock> startTime,
        std::chrono::duration<double, std::milli> frameTime) -> void
{
    std::chrono::duration<double, std::milli> diff{};
    do {
        diff = std::chrono::steady_clock::now() - startTime;
    } while (diff.count() <= frameTime.count());
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
    else if (e.key() == KeyCode::P)
        m_freecell.handleInputPause();
    else if (e.key() == KeyCode::C)
    {
        ResourceManager::recompileShaders();
        m_renderer.reloadShaders();
    }
}

auto Solitaire::onGameWin(const GameWinEvent& e) -> void
{
    m_uiRenderer->showWonWindow();
}

auto Solitaire::onUiGameEvent(const UiGameEvent& e) -> void
{
    switch (e.event())
    {
        case Event::GameEvent::NewGame:
            m_freecell.handleInputNewGame();
            break;
        case Event::GameEvent::Restart:
            m_freecell.handleInputRestart();
            break;
        case Event::GameEvent::Undo:
            m_freecell.handleInputUndo();
            break;
        case Event::GameEvent::Redo:
            m_freecell.handleInputRedo();
            break;
    }
}

auto Solitaire::onUiRecompileShaderEvent(const UiRecompileShaderEvent& e) -> void
{
    ResourceManager::recompileShaders();
    m_renderer.reloadShaders();
}

auto Solitaire::onUiPrintCardEvent(const UiPrintCardEvent& e) -> void
{
    m_freecell.handleInputPrintCards();
}

