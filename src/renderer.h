#pragma once

#include <glm/vec2.hpp>

#include "freecell.h"
#include "shader.h"
#include "texture.h"
#include "model.h"

enum class RenderMode
{
    Shaded,
    Wireframe,
    ShadedWireframe
};

class Renderer
{
public:
    Renderer();
    auto render(const Board& board, RenderMode mode) -> void;

private:
    auto initMesh() -> void;
    auto initBackgroundMesh() -> void;
    auto createUBO() -> void;
    auto setCameraUBO() -> void;
    auto setShaderUniformBlock() -> void;
    auto setShaderUniforms() -> void;
    auto renderBackground(RenderMode mode) -> void;
    auto renderCardBackground(const Board& board, RenderMode mode) -> void;
    auto renderSprite(Card* card) -> void;
    auto drawCall() -> void;
    auto clear() -> void;

    glm::mat4 m_proj;
    Shader* m_shader;
    Shader* m_unlitShader;
    Shader* m_wireframeShader;
    Texture* m_texture;
    Model* m_model;

    Model* m_backgroundModel;
    Shader* m_backgroundShader;
    Shader* m_backgroundWireframeShader;

    std::vector<Shader*> m_shaders;

    unsigned int m_instanceCounter;

    unsigned int UBO;
    unsigned int VBO, EBO, VAO;
    unsigned int VBO_BG, EBO_BG, VAO_BG;
};
