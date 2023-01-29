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
    void run();

private:
    void init();
    void mainLoop();

    void onMouseClick(const MouseClickEvent& e);
    void onMouseDoubleClick(const MouseDoubleClickEvent& e);
    void onMouseDragStart(const MouseDragStartEvent& e);
    void onMouseDragEnd(const MouseDragEndEvent& e);
    void onKeyboardPress(const KeyboardPressEvent& e);
    void onGameWin(const GameWinEvent& e);
    void onUiNewGameEvent(const UiNewGameEvent& e);

    AppConfig m_appConfig;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;
    Freecell m_freecell;
};
