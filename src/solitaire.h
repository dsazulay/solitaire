#pragma once

#include <memory>
#include <string>

#include "window.h"
#include "renderer.h"
#include "ui_renderer.h"
#include "freecell.h"
#include "event.h"

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

    auto sleepToTargetFps(
            std::chrono::time_point<std::chrono::steady_clock> startTime,
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
    Window m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;
    Freecell m_freecell;
};

