#pragma once

#include "vulkan_engine.h"
#include "particle.h"
#include "../card.h"

enum class RenderMode
{
    Shaded,
    Wireframe,
    ShadedWireframe
};


struct BackgroundUniform
{
    glm::mat4 projection;
    glm::mat4 model;
};

struct CardUniform
{
    glm::mat4 projection;
    glm::mat4 model[60];
    glm::vec2 uvOffset[60];
};


struct ParticleUniform
{
    glm::mat4 projection;
    glm::mat4 model[300];
    glm::vec2 offset[300];
    glm::vec4 color[300];
};

class VulkanRenderer
{
public:
    auto init(VulkanEngine* vulkanEngine) -> void;
    auto render(
        const std::span<CardEntity*> cards, const std::span<CardBg> cardBgs,
        const std::span<ParticleSystem> partciles, RenderMode mode) -> void;
    auto terminate() -> void;
private:
    auto updateBackgroundUniform(glm::mat4 model) -> void;
    auto updateCardUniform(const std::span<CardEntity*> cards) -> void;
    auto updateCardBgUniform(const std::span<CardBg> cards) -> void;
    auto updateParticleUniform(const std::span<ParticleSystem> particles) -> void;

    glm::mat4 m_proj;
    Transform m_psTransform;

    BackgroundUniform m_backgroundUniform;
    CardUniform m_cardUniform;
    CardUniform m_cardBgUniform;
    ParticleUniform m_particleUniform;

    size_t cardGO;
    size_t cardBgGO;
    size_t particleGO;

    VulkanEngine* m_vulkanEngine;
};

