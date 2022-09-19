#pragma once

#include "shader.h"
#include "texture.h"

#include <glm/mat4x4.hpp>

class Renderer
{
public:
    void init();
    void render();
    void terminate();
    void clear();
private:
    void initMesh();
    void renderSprite();

    glm::mat4 m_proj;
    Shader* m_shader;
    Texture* m_texture;

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
