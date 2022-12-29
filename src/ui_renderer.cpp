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

    if (m_shouldRenderDebugWindow)
        renderDebugWindow();

    if (m_shouldRenderWonWindow)
        renderWonWindow();

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

void UiRenderer::renderWonWindow()
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 320, main_viewport->WorkPos.y + 120));
    ImGui::SetNextWindowSize(ImVec2(640, 480));
    
    ImGui::Begin("Won window");
    ImGui::Text("You won!");
    if (ImGui::Button("Play again"))
    {
        hideWonWindow();
        UiNewGameEvent e;
        Dispatcher::instance().post(e);
    }
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
