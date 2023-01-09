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
    ~UiRenderer();
    void render();
    int renderMode();
    void showWonWindow();
    void hideWonWindow();
    void showStatsWindow();
    void hideStatsWindow();
    void toggleStatsWindow();
    void showDebugWindow();
    void hideDebugWindow();
    void toggleDebugWindow();
    void setPlayerAndMatchData(PlayerData* playerData, MatchData* matchInfo);

private:
    void renderWonWindow();
    void renderStatsWindow();
    void renderTimeWindow();
    void renderDebugWindow();
    
    void toggleWonWindow();
    void toggleImguiDemoWindow();

    void initStyleValues();
    void pushCommonStyle();
    void popCommonStyle();
    void pushPopupWindowStyle();
    void popPopupWindowStyle();

    int m_renderMode{};
    bool m_shouldRenderWonWindow{};
    bool m_shouldRenderDebugWindow{};
    bool m_shouldRenderStatsWindow{};
    bool m_shouldRenderImguiDemoWindow{};
    WindowConfig m_statsWindowConfig{};
    PlayerData* m_playerData;
    MatchData* m_matchData;
};
