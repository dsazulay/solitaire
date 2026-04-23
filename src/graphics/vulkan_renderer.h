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

class VulkanRenderer
{
public:
    auto init(GLFWwindow* window) -> void;
    auto render(const std::span<CardEntity*> cards, const std::span<CardBg> cardBgs,
            const std::span<ParticleSystem> partciles, RenderMode mode) -> void;
    auto terminate() -> void;
    auto getVulkanPointers() -> VulkanPointers;
    auto waitDevice() -> void;
private:
    auto updateBackgroundUniform(glm::mat4 model) -> void;
    auto updateCardUniform(const std::span<CardEntity*> cards) -> void;
    auto updateCardBgUniform(const std::span<CardBg> cards) -> void;

    VulkanEngine m_vulkanEngine;
    glm::mat4 m_proj;
    BackgroundUniform m_backgroundUniform;
    CardUniform m_cardUniform;
    CardUniform m_cardBgUniform;

    size_t cardGO;
    size_t cardBgGO;
};

