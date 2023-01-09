#include "ui_renderer.h"

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

    initStyleValues();
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

    pushCommonStyle();

    renderTimeWindow();

    if (m_shouldRenderWonWindow)
        renderWonWindow();
    
    if (m_shouldRenderStatsWindow)
        renderStatsWindow();

    popCommonStyle();

    if (m_shouldRenderDebugWindow)
        renderDebugWindow();

    if (m_shouldRenderImguiDemoWindow)
        ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int UiRenderer::renderMode()
{
    return m_renderMode;
}

void UiRenderer::showWonWindow()
{
    m_shouldRenderWonWindow = true;
}

void UiRenderer::hideWonWindow()
{
    m_shouldRenderWonWindow = false;
}

void UiRenderer::showStatsWindow()
{
    m_shouldRenderStatsWindow = true;
}

void UiRenderer::hideStatsWindow()
{
    m_shouldRenderStatsWindow = false;
}

void UiRenderer::toggleStatsWindow()
{
    m_shouldRenderStatsWindow = !m_shouldRenderStatsWindow;
}

void UiRenderer::showDebugWindow()
{
    m_shouldRenderDebugWindow = true;
}

void UiRenderer::hideDebugWindow()
{
    m_shouldRenderDebugWindow = false;
}

void UiRenderer::toggleDebugWindow()
{
    m_shouldRenderDebugWindow = !m_shouldRenderDebugWindow;
}

void UiRenderer::toggleWonWindow()
{
    m_shouldRenderWonWindow = !m_shouldRenderWonWindow;
}

void UiRenderer::toggleImguiDemoWindow()
{
    m_shouldRenderImguiDemoWindow = !m_shouldRenderImguiDemoWindow;
}

void UiRenderer::setPlayerAndMatchData(PlayerData* playerData, MatchData* matchData)
{
    m_playerData = playerData;
    m_matchData = matchData;
}

void UiRenderer::renderWonWindow()
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 400, main_viewport->WorkPos.y + 180));
    ImGui::SetNextWindowSize(ImVec2(480, 360));
    
    pushPopupWindowStyle();
    ImGui::Begin("Won window", &m_shouldRenderWonWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("You won!"); ImGui::SameLine(ImGui::GetContentRegionAvail().x + 30);
    if (ImGui::Button("X", ImVec2{24, 24}))
    {
        hideWonWindow();
    }
    // ImGui::Spacing();
    if (ImGui::BeginTable("Stats", 2))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Time"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d:%d", (int)m_matchData->currentTime / 60, (int)m_matchData->currentTime % 60);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Games Played"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d", m_playerData->gamesPlayed);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Games Won"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d", m_playerData->gamesWon);
       
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Separator();
        ImGui::TableNextColumn();
        ImGui::Separator();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Best Time"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d:%d", (int)m_playerData->bestTime / 60, (int)m_playerData->bestTime % 60);
        ImGui::EndTable();
    }
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60);
    if (ImGui::Button("Play again", ImVec2{110, 28}))
    {
        hideWonWindow();
        UiNewGameEvent e;
        Dispatcher::instance().post(e);
    }
    popPopupWindowStyle();
    ImGui::End();
}

void UiRenderer::renderStatsWindow()
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 440, main_viewport->WorkPos.y + 210));
    ImGui::SetNextWindowSize(ImVec2(400, 300));

    pushPopupWindowStyle();
    ImGui::Begin("Stats window", &m_shouldRenderStatsWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("STATISTICS"); ImGui::SameLine(ImGui::GetContentRegionAvail().x + 30);
    if (ImGui::Button("X", ImVec2{24, 24}))
    {
        hideStatsWindow();
    }

    if (ImGui::BeginTable("Stats", 2))
    {
        ImGui::Indent(30.0f);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Games Played"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d", m_playerData->gamesPlayed);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Games Won"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d", m_playerData->gamesWon);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Percentage Won"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d%%", (int)((float)m_playerData->gamesWon / m_playerData->gamesPlayed * 100));
       
        ImGui::Unindent(30.0f);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Separator();
        ImGui::TableNextColumn();
        ImGui::Separator();

        ImGui::Indent(30.0f);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Best Time"); ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::Text("%d:%d", (int)m_playerData->bestTime / 60, (int)m_playerData->bestTime % 60);
        ImGui::EndTable();
    }
    
    popPopupWindowStyle();
    ImGui::End();
}

void UiRenderer::renderTimeWindow()
{
    bool isOpen;
    ImGui::Begin("Time", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("TIME: %d:%d", (int)m_matchData->currentTime / 60, (int)m_matchData->currentTime % 60);
    ImGui::End();
}

void UiRenderer::renderDebugWindow()
{
    ImGui::Begin("Debug");
    ImGui::Text("Frame time: %.3fms", Timer::deltaTime);
    ImGui::Text("FPS: %d", (int)(1 / Timer::deltaTime));
    ImGui::Combo("Render mode", &m_renderMode, "Shaded\0Wireframe\0Shaded Wireframe\0\0");
    ImGui::Separator();
    ImGui::Text("Stats Window Configuraion");
    ImGui::SliderFloat2("Window Padding", (float*)&m_statsWindowConfig.windowPadding, 0.0f, 100.0f);
    ImGui::SliderFloat2("Item Spacing", (float*)&m_statsWindowConfig.itemSpacing, 0.0f, 100.0f);
    ImGui::SliderFloat2("Cell Padding", &m_statsWindowConfig.cellPadding.x, 0.0f, 40.0f, "%.0f");
    ImGui::Separator();
    if (ImGui::Button("Won Window"))
    {
        toggleWonWindow();
    }
    if (ImGui::Button("Imgui Demo Window"))
    {
        toggleImguiDemoWindow();
    }

    ImGui::End();
}

void UiRenderer::initStyleValues()
{
    m_statsWindowConfig.windowPadding = ImVec2{40.0f, 31.0f};
    m_statsWindowConfig.itemSpacing = ImVec2{0.0f, 21.0f};
    m_statsWindowConfig.cellPadding = ImVec2{18.0f, 12.0f};
}

void UiRenderer::pushCommonStyle()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.22f, 0.49f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.45f, 0.72f, 0.57f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.45f, 0.72f, 0.57f, 1.0f));
}

void UiRenderer::popCommonStyle()
{
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
}

void UiRenderer::pushPopupWindowStyle()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_statsWindowConfig.windowPadding);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_statsWindowConfig.itemSpacing);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, m_statsWindowConfig.cellPadding);
    // Button style
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
}

void UiRenderer::popPopupWindowStyle()
{
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(5);
}
