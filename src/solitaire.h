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
};

class Solitaire
{
public:
    Solitaire();
    auto run() -> void;

private:
    auto init() -> void;
    auto mainLoop() -> void;

    auto onMouseClick(const MouseClickEvent& e) -> void;
    auto onMouseDoubleClick(const MouseDoubleClickEvent& e) -> void;
    auto onMouseDrag(const MouseDragEvent& e) -> void;
    auto onKeyboardPress(const KeyboardPressEvent& e) -> void;
    auto onGameWin(const GameWinEvent& e) -> void;
    auto onUiNewGameEvent(const UiNewGameEvent& e) -> void;

    AppConfig m_appConfig;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;
    Freecell m_freecell;

    constexpr static int DEFAULT_WINDOW_WIDTH = 1280;
    constexpr static int DEFAULT_WINDOW_HEIGHT = 720;
};
