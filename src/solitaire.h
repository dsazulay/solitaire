#pragma once

#include "graphics/vulkan_engine.h"
#include "window.h"
#include "utils/types.h"
#include "event.h"
#include "graphics/vulkan_renderer.h"
#include "ui_renderer.h"
#include "animation/animation_engine.h"
#include "game_common/igame_handler.h"
#include "freecell/freecell.h"
#include "scoundrel/scoundrel.h"

#include <string>

struct AppConfig
{
    std::string windowName;
    i32 windowWidth{};
    i32 windowHeight{};
    f64 fps{};
    f64 idleFps{};
};

class Solitaire
{
public:
    auto run() -> void;

private:
    auto init() -> void;
    auto terminate() -> void;
    auto mainLoop() -> void;

    auto sleepToTargetFps(
            std::chrono::time_point<std::chrono::steady_clock> startTime,
            std::chrono::duration<double, std::milli> frameTime
    ) -> void;

    auto onMouseClick(const MouseClickEvent& e) -> void;
    auto onMouseDoubleClick(const MouseDoubleClickEvent& e) -> void;
    auto onMouseDrag(const MouseDragEvent& e) -> void;
    auto onKeyboardPress(const KeyboardPressEvent& e) -> void;
    auto onGameWin(const GameWinEvent& e) -> void;
    auto onUiGameEvent(const UiGameEvent& e) -> void;
    auto onUiRecompileShaderEvent(const UiRecompileShaderEvent& e) -> void;
    auto onUiPrintCardEvent(const UiPrintCardEvent& e) -> void;
    auto onUiRestartParticlesEvent(const UiRestartParticlesEvent& e) -> void;

    AppConfig m_appConfig;
    Window m_window;
    VulkanEngine m_vulkanEngine;
    VulkanRenderer m_vulkanRenderer;
    UiRenderer m_uiRenderer;
    AnimationEngine m_animationEngine;
    Freecell m_freecell;
    Scoundrel m_scoundrel;

    IInputHandler* gameInputHandler;
    IGameHandler* gameHandler;

    std::vector<ParticleSystem> m_ps;
};

