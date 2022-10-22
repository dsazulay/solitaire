#pragma once

#include "shader.h"
#include "texture.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Card
{
    unsigned int number;
    unsigned int suit;
    uint8_t color;
    glm::vec3 selectionTint;
};


class Renderer
{
public:
    void init();
    void render(const glm::vec2 (&map)[8][12], std::vector<Card*>* deck);
    void renderOpenCellsAndFoundation(const glm::vec2 (&map)[4], std::vector<Card*>* pile);
    void terminate();
    void clear();
private:
    void initMesh();
    void renderSprite(glm::vec2 pos, Card* card);

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
