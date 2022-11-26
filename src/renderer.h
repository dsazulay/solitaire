#pragma once

#include "freecell.h"
#include "shader.h"
#include "texture.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

enum class RenderMode
{
    Shaded,
    Wireframe,
    ShadedWireframe
};

class Renderer
{
public:
    void init();
    void render(const Board& board, RenderMode mode);

private:
    void initMesh();
    void renderSprite(glm::vec2 pos, Card* card);
    void drawCall();
    void clear();
    void setDragOffset(glm::vec2 pos);

    glm::mat4 m_proj;
    Shader* m_shader;
    Shader* m_unlitShader;
    Shader* m_wireframeShader;
    Texture* m_texture;

    glm::vec2 dragOffset;

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
