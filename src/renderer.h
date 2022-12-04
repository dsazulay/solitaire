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
    void render(const Board& board, RenderMode mode);

private:
    void initMesh();
    void renderBackground(const Board& board, RenderMode mode);
    void renderSprite(glm::vec2 pos, Card* card);
    void drawCall();
    void clear();

    glm::mat4 m_proj;
    Shader* m_shader;
    Shader* m_unlitShader;
    Shader* m_wireframeShader;
    Texture* m_texture;
    Model* m_model;

    unsigned int m_instanceCounter;

    unsigned int VBO, EBO, VAO;

    static constexpr float m_vertices[16] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    static constexpr unsigned int m_indices[6] = {
        0, 1, 2, 2, 3, 0
    };
};
