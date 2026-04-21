#include "vulkan_renderer.h"

#include "resource_manager.h"
#include "../components.h"
#include "vulkan_engine.h"
#include <cstddef>
#include <glm/glm.hpp>

constexpr const char* CARD_MODEL_PATH = "resources/card.obj";
constexpr const char* BG_SHADER_PATH = "assets/background.slang";
constexpr const char* CARD_SHADER_PATH = "assets/card.slang";

constexpr const float WIDTH = 1280.0;
constexpr const float HEIGHT = 720.0;

constexpr const glm::vec3 BG_POS{ 640.0, 360.0, -0.1f };
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

    cardGO = m_vulkanEngine.addGameObject(cardID, cardPipelineID);
    size_t bgGO = m_vulkanEngine.addGameObject(bgID, bgPipelineID);


    m_proj = glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f);
    Transform bgTransform;
    bgTransform.pos(BG_POS);
    bgTransform.scale(BG_SCALE);

    updateBackgroundUniform(bgTransform.model());


    m_vulkanEngine.setUniformData(bgGO, &m_backgroundUniform, sizeof(BackgroundUniform));

    m_vulkanEngine.setUniformData(cardGO, &m_cardUniform, sizeof(CardUniform));

    m_vulkanEngine.createUniformBuffers();
}

auto VulkanRenderer::render(const std::span<CardEntity*> cards, const std::span<CardBg> cardBgs,
            const std::span<ParticleSystem> partciles, RenderMode mode) -> void
{
    m_vulkanEngine.updateGameObjectInstanceCount(cardGO, cards.size());
    updateCardUniform(cards);
    m_vulkanEngine.render();
}

auto VulkanRenderer::terminate() -> void
{
    m_vulkanEngine.terminate();
}

auto VulkanRenderer::updateBackgroundUniform(glm::mat4 model) -> void
{
    m_backgroundUniform.projection = m_proj;
    m_backgroundUniform.model = model;
}

auto VulkanRenderer::updateCardUniform(const std::span<CardEntity*> cards) -> void
{
    m_cardUniform.projection = m_proj;

    size_t index = 0;
    for (CardEntity* card : cards)
    {
        m_cardUniform.model[index] = card->transform.model();
        m_cardUniform.uvOffset[index] = card->sprite.uv;
        index++;
    }
}
