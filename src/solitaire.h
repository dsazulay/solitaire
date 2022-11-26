#pragma once

#include <memory>
#include <string>

#include "window.h"
#include "freecell.h"
#include "ui_renderer.h"
#include "renderer.h"
#include "event.h"

struct AppConfig
{
    std::string windowName;
    uint32_t windowWidth;
    uint32_t windowHeight;
};

class Solitaire
{
public:
    Solitaire();
    void run();

private:
    void init();
    void mainLoop();
    void terminate();
    void onMouseClick(const Event& e);
    void onMouseDoubleClick(const Event& e);
    void onMouseDragStart(const Event& e);
    void onMouseDragEnd(const Event& e);

    AppConfig m_appConfig;
    Window* m_window;
    Renderer* m_renderer;
    std::unique_ptr<UiRenderer> m_uiRenderer;

    Freecell m_freecell;
};

