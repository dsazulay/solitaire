#include "ui_renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "timer.h"
#include "dispatcher.h"
#include "event.h"
#include "utils/log.h"

UiRenderer::UiRenderer(GLFWwindow* window)
{
    const char* glsl_version = "#version 410";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    showDebugWindow();
}

UiRenderer::~UiRenderer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UiRenderer::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderTimeWindow();

    if (m_shouldRenderWonWindow)
        renderWonWindow();

    if (m_shouldRenderDebugWindow)
        renderDebugWindow();

    // ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int UiRenderer::renderMode()
{
    return m_renderMode;
}

void UiRenderer::showDebugWindow()
{
    m_shouldRenderDebugWindow = true;
}

void UiRenderer::hideDebugWindow()
{
    m_shouldRenderDebugWindow = false;
}

void UiRenderer::showWonWindow()
{
    m_shouldRenderWonWindow = true;
}

void UiRenderer::hideWonWindow()
{
    m_shouldRenderWonWindow = false;
}

void UiRenderer::setPlayerAndMatchData(PlayerData* playerData, MatchData* matchData)
{
    m_playerData = playerData;
    m_matchData = matchData;
}

void UiRenderer::renderWonWindow()
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 320, main_viewport->WorkPos.y + 120));
    ImGui::SetNextWindowSize(ImVec2(640, 480));
    
    ImGui::Begin("Won window");
    ImGui::Text("You won!");
    ImGui::Text("Time: %d:%d", (int)m_matchData->currentTime / 60, (int)m_matchData->currentTime % 60);
    ImGui::Text("Games Played %d", m_playerData->gamesPlayed);
    ImGui::Text("Games Won %d", m_playerData->gamesWon);
    ImGui::Text("Best Time %d:%d", (int)m_playerData->bestTime / 60, (int)m_playerData->bestTime % 60);
    if (ImGui::Button("Play again"))
    {
        hideWonWindow();
        UiNewGameEvent e;
        Dispatcher::instance().post(e);
    }
    ImGui::End();
}

void UiRenderer::renderStatsWindow()
{
    ImGui::Begin("Stats window");
    ImGui::Text("Stats");
    ImGui::Text("Games Played %d", m_playerData->gamesPlayed);
    ImGui::Text("Games Won %d", m_playerData->gamesWon);
    ImGui::Text("Percentage Won %d%%", (int)((float)m_playerData->gamesWon / m_playerData->gamesPlayed * 100));
    ImGui::Text("Best Time %d:%d", (int)m_playerData->bestTime / 60, (int)m_playerData->bestTime % 60);
}

void UiRenderer::renderTimeWindow()
{
    ImGui::Begin("Time");
    ImGui::Text("TIME: %d:%d", (int)m_matchData->currentTime / 60, (int)m_matchData->currentTime % 60);
    ImGui::End();
}

void UiRenderer::renderDebugWindow()
{
    ImGui::Begin("Debug");
    ImGui::Text("Frame time: %.3fms", Timer::deltaTime);
    ImGui::Text("FPS: %d", (int)(1 / Timer::deltaTime));
    ImGui::Combo("Render mode", &m_renderMode, "Shaded\0Wireframe\0Shaded Wireframe\0\0");
    ImGui::End();
}
