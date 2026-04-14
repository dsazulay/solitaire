#include "vulkan_renderer.h"

#include "resource_manager.h"
#include "../components.h"

constexpr const char* CARDMODELPATH = "resources/card.obj";
constexpr const char* BGSHADERPATH = "assets/background.slang";

constexpr const float WIDTH = 1280.0;
constexpr const float HEIGHT = 720.0;

constexpr const glm::vec3 BG_POS{ 640.0, 360.0, -0.01 };
constexpr const glm::vec2 BG_SCALE{ 640, 360 };

auto VulkanRenderer::init(GLFWwindow* window) -> void
{
    m_vulkanEngine.init(window);

    Model* cardModel = ResourceManager::loadModel(CARDMODELPATH, "CardModel");
    Model* backgroundModel = ResourceManager::loadModel(NativeModel::Quad, "QuadModel");

    Shader* bgShader = ResourceManager::loadShader(BGSHADERPATH, "BGShader");

    //m_vulkanEngine.loadMeshData(cardModel->vertices, cardModel->indices);
    m_vulkanEngine.loadMeshData(backgroundModel->vertices, backgroundModel->indices);

    m_vulkanEngine.loadShader(bgShader->bufferSize, bgShader->bufferPointer);

    m_vulkanEngine.createPipeline();


    glm::mat4 proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
    Transform bgTransform;
    bgTransform.pos(BG_POS);
    bgTransform.scale(BG_SCALE);

    m_vulkanEngine.setUniformData(proj, bgTransform.model());
}

auto VulkanRenderer::render() -> void
{
    m_vulkanEngine.render();
}

auto VulkanRenderer::terminate() -> void
{
    m_vulkanEngine.terminate();
}
