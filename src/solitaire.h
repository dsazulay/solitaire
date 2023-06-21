#pragma once

#include <memory>
#include <string>

#include "window.h"
#include "renderer.h"
#include "ui_renderer.h"
#include "freecell.h"
#include "event.h"
#include "dispatcher.h"

struct AppConfig
{
    std::string windowName;
    int windowWidth{};
    int windowHeight{};
    double fps{};
    double idleFps{};
};

class Solitaire
{
public:
    Solitaire();
    auto run() -> void;

private:
    auto init() -> void;
    auto mainLoop() -> void;

    auto sleepToTargetFps(std::chrono::time_point<std::chrono::steady_clock> startTime,
            std::chrono::duration<double, std::milli> frameTime) -> void;

    auto onMouseClick(const MouseClickEvent& e) -> void;
    auto onMouseDoubleClick(const MouseDoubleClickEvent& e) -> void;
    auto onMouseDrag(const MouseDragEvent& e) -> void;
    auto onKeyboardPress(const KeyboardPressEvent& e) -> void;
    auto onGameWin(const GameWinEvent& e) -> void;
    auto onUiGameEvent(const UiGameEvent& e) -> void;
    auto onUiRecompileShaderEvent(const UiRecompileShaderEvent& e) -> void;
    auto onUiPrintCardEvent(const UiPrintCardEvent& e) -> void;

    AppConfig m_appConfig;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;
    Freecell m_freecell;

    constexpr static const int defaultWindowWidth = 1280;
    constexpr static const int defaultWindowHeight = 720;
    constexpr static const double fps = 60.0;
    constexpr static const double idleFps = 5.0;
};
