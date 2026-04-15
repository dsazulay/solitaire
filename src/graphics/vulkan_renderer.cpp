#include "vulkan_renderer.h"

#include "resource_manager.h"
#include "../components.h"
#include "vulkan_engine.h"
#include <cstddef>

constexpr const char* CARD_MODEL_PATH = "resources/card.obj";
constexpr const char* BG_SHADER_PATH = "assets/background.slang";
constexpr const char* CARD_SHADER_PATH = "assets/card.slang";

constexpr const float WIDTH = 1280.0;
constexpr const float HEIGHT = 720.0;

constexpr const glm::vec3 BG_POS{ 640.0, 360.0, -0.01 };
constexpr const glm::vec2 BG_SCALE{ 640, 360 };

auto VulkanRenderer::init(GLFWwindow* window) -> void
{
    m_vulkanEngine.init(window);

    Model* cardModel = ResourceManager::loadModel(CARD_MODEL_PATH, "CardModel");
    Model* backgroundModel = ResourceManager::loadModel(NativeModel::Quad, "QuadModel");

    Shader* bgShader = ResourceManager::loadShader(BG_SHADER_PATH, "BGShader");
    Shader* cardShader = ResourceManager::loadShader(CARD_SHADER_PATH, "CardShader");

    size_t bgID = m_vulkanEngine.loadMeshData(backgroundModel->vertices, backgroundModel->indices);
    size_t cardID = m_vulkanEngine.loadMeshData(cardModel->vertices, cardModel->indices);

    ShaderID bgShaderID = m_vulkanEngine.loadShader(bgShader->bufferSize, bgShader->bufferPointer);
    ShaderID cardShaderID = m_vulkanEngine.loadShader(cardShader->bufferSize, cardShader->bufferPointer);

    PipelineID bgPipelineID = m_vulkanEngine.createPipeline(bgShaderID);
    PipelineID cardPipelineID = m_vulkanEngine.createPipeline(cardShaderID);

    size_t bgGO = m_vulkanEngine.addGameObject(bgID, bgPipelineID);
    size_t cardGO = m_vulkanEngine.addGameObject(cardID, cardPipelineID);

    m_vulkanEngine.createUniformBuffers();

    glm::mat4 proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
    Transform bgTransform;
    bgTransform.pos(BG_POS);
    bgTransform.scale(BG_SCALE);

    m_vulkanEngine.setUniformData(bgGO, proj, bgTransform.model());

    Transform cardTransform;
    cardTransform.pos(BG_POS);
    m_vulkanEngine.setUniformData(cardGO, proj, cardTransform.model());
}

auto VulkanRenderer::render() -> void
{
    m_vulkanEngine.render();
}

auto VulkanRenderer::terminate() -> void
{
    m_vulkanEngine.terminate();
}
