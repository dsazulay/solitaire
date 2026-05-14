#include "vulkan_renderer.h"

#include "particle.h"
#include "resource_manager.h"
#include "../components.h"
#include "vulkan_engine.h"
#include <cstddef>
#include <glm/glm.hpp>

constexpr const char* CARD_MODEL_PATH = "assets/card.obj";
constexpr const char* BG_SHADER_PATH = "assets/background.slang";
constexpr const char* CARD_SHADER_PATH = "assets/card.slang";
constexpr const char* PARTICLE_SHADER_PATH = "assets/particle.slang";

constexpr const float WIDTH = 1280.0;
constexpr const float HEIGHT = 720.0;

constexpr const glm::vec3 BG_POS{ 640.0, 360.0, -0.1f };
constexpr const glm::vec2 BG_SCALE{ 640, 360 };

auto VulkanRenderer::init(VulkanEngine* vulkanEngine) -> void
{
    m_vulkanEngine = vulkanEngine;
    Model* cardModel = ResourceManager::loadModel(CARD_MODEL_PATH, "CardModel");
    Model* backgroundModel = ResourceManager::loadModel(NativeModel::Quad, "QuadModel");

    Shader* bgShader = ResourceManager::loadShader(BG_SHADER_PATH, "BGShader");
    Shader* cardShader = ResourceManager::loadShader(CARD_SHADER_PATH, "CardShader");
    Shader* particleShader = ResourceManager::loadShader(PARTICLE_SHADER_PATH, "ParticleShader");

    MeshID bgID = m_vulkanEngine->loadMeshData(backgroundModel->vertices, backgroundModel->indices);
    MeshID cardID = m_vulkanEngine->loadMeshData(cardModel->vertices, cardModel->indices);

    ShaderID bgShaderID = m_vulkanEngine->loadShader(bgShader->bufferSize, bgShader->bufferPointer);
    ShaderID cardShaderID = m_vulkanEngine->loadShader(cardShader->bufferSize, cardShader->bufferPointer);
    ShaderID particleShaderID = m_vulkanEngine->loadShader(particleShader->bufferSize, particleShader->bufferPointer);

    PipelineID bgPipelineID = m_vulkanEngine->createPipeline(bgShaderID);
    PipelineID cardPipelineID = m_vulkanEngine->createPipeline(cardShaderID);
    PipelineID cardBgPipelineID = m_vulkanEngine->createPipeline(cardShaderID, Blending::ALPHA_BLEND);
    PipelineID particlePipelineID = m_vulkanEngine->createPipeline(particleShaderID, Blending::ALPHA_BLEND);

    m_shaderPipelineMap.push_back(
        { bgShader, bgShaderID, { { bgPipelineID } } }
    );
    m_shaderPipelineMap.push_back(
        { cardShader, cardShaderID, { { cardPipelineID }, { cardBgPipelineID, Blending::ALPHA_BLEND } } }
    );
    m_shaderPipelineMap.push_back(
        { particleShader, particleShaderID, { { particlePipelineID, Blending::ALPHA_BLEND } } }
    );

    GameObjectID bgGO = m_vulkanEngine->addGameObject(bgID, bgPipelineID);
    cardBgGO = m_vulkanEngine->addGameObject(cardID, cardBgPipelineID);
    cardGO = m_vulkanEngine->addGameObject(cardID, cardPipelineID);
    particleGO = m_vulkanEngine->addGameObject(bgID, particlePipelineID);


    m_proj = glm::ortho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f);
    Transform bgTransform;
    bgTransform.pos(BG_POS);
    bgTransform.scale(BG_SCALE);

    updateBackgroundUniform(bgTransform.model());

    m_psTransform.pos(glm::vec3{ 640, 360, 0.0 });
    m_psTransform.scale(glm::vec2{ 4, 4 });

    m_vulkanEngine->setUniformData(bgGO, &m_backgroundUniform, sizeof(BackgroundUniform));
    m_vulkanEngine->setUniformData(cardGO, &m_cardUniform, sizeof(CardUniform));
    m_vulkanEngine->setUniformData(cardBgGO, &m_cardBgUniform, sizeof(CardUniform));
    m_vulkanEngine->setUniformData(particleGO, &m_particleUniform, sizeof(ParticleUniform));

    m_vulkanEngine->createUniformBuffers();
}

auto VulkanRenderer::render(
    const std::span<CardEntity*> cards, const std::span<CardBg> cardBgs,
    const std::span<ParticleSystem> particles, RenderMode mode) -> void
{
    updateCardUniform(cards);
    updateCardBgUniform(cardBgs);
    updateParticleUniform(particles);
    m_vulkanEngine->render();
}

auto VulkanRenderer::terminate() -> void
{
}

auto VulkanRenderer::reloadShaders() -> void
{
    ResourceManager::recompileShaders();
    for (ShaderPipelineMap& map : m_shaderPipelineMap)
    {
        if (map.shader->reloaded)
        {
            map.shader->reloaded = false;
            m_vulkanEngine->reloadShader(map.shaderID, map.shader->bufferSize, map.shader->bufferPointer);
            for (PipelineMap& pipeline : map.pipelines)
            {
                m_vulkanEngine->reloadPipeline(pipeline.id, map.shaderID, pipeline.blending);
            }
        }
    }
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
    m_vulkanEngine->updateGameObjectInstanceCount(cardGO, cards.size());
}

auto VulkanRenderer::updateCardBgUniform(const std::span<CardBg> cards) -> void
{
    m_cardBgUniform.projection = m_proj;

    size_t index = 0;
    for (CardBg card : cards)
    {
        m_cardBgUniform.model[index] = card.transform.model();
        m_cardBgUniform.uvOffset[index] = card.sprite.uv;
        index++;
    }
    m_vulkanEngine->updateGameObjectInstanceCount(cardBgGO, cards.size());
}

auto VulkanRenderer::updateParticleUniform(const std::span<ParticleSystem> particles) -> void
{
    m_particleUniform.projection = m_proj;

    size_t index = 0;
    for (ParticleSystem& ps : particles)
    {
        for (const Particle& p : ps.particles())
        {
            if (p.life > 0.0)
            {
                m_particleUniform.model[index] = m_psTransform.model();
                m_particleUniform.offset[index] = p.pos;
                m_particleUniform.color[index] = p.color;
                index++;
            }
        }
    }
    m_vulkanEngine->updateGameObjectInstanceCount(particleGO, index);
}

