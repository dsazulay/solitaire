#pragma once

#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <imgui.h>

#include "gamedata.h"

struct WindowConfig
{
    ImVec2 windowPadding;
    ImVec2 itemSpacing;
    ImVec2 cellPadding;
};

class UiRenderer
{
public:
    UiRenderer(GLFWwindow* window);
    UiRenderer(const UiRenderer& u) = delete;
    UiRenderer(UiRenderer&& u) = delete;
    auto operator=(const UiRenderer&) -> UiRenderer& = delete;
    auto operator=(UiRenderer&&) -> UiRenderer& = delete;
    ~UiRenderer();

    void render();
    auto renderMode() -> int;
    auto showWonWindow() -> void;
    auto hideWonWindow() -> void;
    auto showStatsWindow() -> void;
    auto hideStatsWindow() -> void;
    auto toggleStatsWindow() -> void;
    auto showDebugWindow() -> void;
    auto hideDebugWindow() -> void;
    auto toggleDebugWindow() -> void;
    auto setPlayerAndMatchData(PlayerData* playerData, MatchData* matchInfo) -> void;

private:
    auto renderWonWindow() -> void;
    auto renderStatsWindow() -> void;
    auto renderTimeWindow() -> void;
    auto renderDebugWindow() -> void;
    
    auto toggleWonWindow() -> void;
    auto toggleImguiDemoWindow() -> void;

    auto initStyleValues() -> void;
    auto pushCommonStyle() -> void;
    auto popCommonStyle() -> void;
    auto pushPopupWindowStyle() -> void;
    auto popPopupWindowStyle() -> void;

    int m_renderMode{};
    bool m_shouldRenderWonWindow{};
    bool m_shouldRenderDebugWindow{};
    bool m_shouldRenderStatsWindow{};
    bool m_shouldRenderImguiDemoWindow{};
    WindowConfig m_statsWindowConfig{};
    PlayerData* m_playerData{};
    MatchData* m_matchData{};
};
