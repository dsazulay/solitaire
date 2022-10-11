#include "renderer.h"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "resource_manager.h"
#include "utils/log.h"
#include <vector>

void Renderer::init()
{
    initMesh();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
    m_shader = ResourceManager::loadShader("../resources/unlit.vert", "../resources/unlit.frag", "Unlit");
    m_texture = ResourceManager::loadTexture("../resources/cards_alpha.png", "cards");
}

void Renderer::render(const glm::vec2 (&map)[8][8], std::vector<Card>* deck)
{
    clear();

    for (int j = 0; j < 8; j ++)
    {
        for (int i = 0; i < deck[j].size(); i++)
        {
            renderSprite(map[j][i], deck[j].at(i));
        }
    }
}

void Renderer::renderSprite(glm::vec2 pos, Card card)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
    model = glm::scale(model, glm::vec3(56, 80, 1));

    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    m_shader->use();
    m_shader->setMat4("u_proj", m_proj);
    m_shader->setMat4("u_model", model);
    m_shader->setVec2("u_offset", card.number, card.suit);
    m_shader->setInt("u_main_tex", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::terminate()
{

}

void Renderer::initMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

void Renderer::clear()
{
    glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
