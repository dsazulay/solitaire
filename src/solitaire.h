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
    unsigned int windowWidth;
    unsigned int windowHeight;
};

class Solitaire
{
public:
    Solitaire();
    void run();

private:
    void init();
    void mainLoop();

    void onMouseClick(const Event& e);
    void onMouseDoubleClick(const Event& e);
    void onMouseDragStart(const Event& e);
    void onMouseDragEnd(const Event& e);
    void onKeyboardPress(const Event& e);
    void onGameWin(const Event& e);
    void onUiNewGameEvent(const Event& e);

    template <class Fp>
    void bindCallback(const char* descriptor, Fp funcPointer);

    AppConfig m_appConfig;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;
    Freecell m_freecell;
};

template <class Fp>
void Solitaire::bindCallback(const char* descriptor, Fp funcPointer)
{
    Dispatcher::instance().subscribe(descriptor, 
        std::bind(funcPointer, this, std::placeholders::_1));
}

