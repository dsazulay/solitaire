#pragma once

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "gamedata.h"

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

    int m_renderMode{};
    bool m_shouldRenderWonWindow{};
    bool m_shouldRenderDebugWindow{};
    bool m_shouldRenderStatsWindow{};
    PlayerData* m_playerData;
    MatchData* m_matchData;
};
