#pragma once

#include <span>
#include <glm/vec2.hpp>

#include "shader.h"
#include "texture.h"
#include "model.h"
#include "../card.h"

enum class RenderMode
{
    Shaded,
    Wireframe,
    ShadedWireframe
};

class Renderer
{
public:
    auto init() -> void;
    auto render(std::span<CardEntity*> cards, std::span<CardBg*> cardsBg,
            RenderMode mode) -> void;
    auto reloadShaders() -> void;

private:
    auto createUBO() -> void;
    auto setCameraUBO() -> void;
    auto setShaderUniformBlock() -> void;
    auto setShaderUniforms() -> void;
    auto renderBackground(RenderMode mode) -> void;
    auto renderSprite(Sprite sprite, const glm::mat4& model) -> void;
    auto drawCall() -> void;
    auto clear() -> void;

    glm::mat4 m_proj;
    Shader* m_shader;
    Shader* m_unlitShader;
    Shader* m_wireframeShader;
    Shader* m_backgroundShader;
    Shader* m_backgroundWireframeShader;

    Texture* m_texture;

    Model* m_model;
    Model* m_backgroundModel;

    unsigned int m_instanceCounter;

    unsigned int UBO;
};
