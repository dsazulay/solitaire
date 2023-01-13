#include "renderer.h"

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "resource_manager.h"
#include "window.h"

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
    m_unlitShader = ResourceManager::loadShader("../../resources/unlit.vert", 
                                                "../../resources/unlit.frag",
                                                "UnlitShader");
    m_wireframeShader = ResourceManager::loadShader("../../resources/unlit.vert",
                                                    "../../resources/wireframe.frag",
                                                    "WireframeShader");
    m_texture = ResourceManager::loadTexture("../../resources/cards.png", "CardTex");

    m_model = ResourceManager::loadModel("../../resources/card.obj", "CardModel");

    initMesh();
    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    m_wireframeShader->use();
    m_wireframeShader->setMat4("u_proj", m_proj);

    m_unlitShader->use();
    m_unlitShader->setMat4("u_proj", m_proj);
    m_unlitShader->setInt("umain_tex", 0);
}

void Renderer::render(const Board& board, RenderMode mode)
{
    clear();

    renderBackground(board, mode);

    glDisable(GL_BLEND);

    if (mode == RenderMode::Shaded || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_unlitShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        for (int j = 0; j < 8; j ++)
        {
            for (int i = 0; i < (int) board.table[j].size(); i++)
            {
                renderSprite(board.table[j].at(i));
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (board.openCells[i].size() == 0)
                continue;
            renderSprite(board.openCells[i].back());
        }

        for (int i = 0; i < 4; i++)
        {
            if (board.foundations[i].size() == 0)
                continue;
            else if(board.foundations[i].size() > 1)
                renderSprite(board.foundations[i][board.foundations[i].size() - 2]);
            renderSprite(board.foundations[i].back());
        }

        drawCall();
    }

    if (mode == RenderMode::Wireframe || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_wireframeShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (int j = 0; j < 8; j ++)
        {
            for (int i = 0; i < (int) board.table[j].size(); i++)
            {
                renderSprite(board.table[j].at(i));
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (board.openCells[i].size() == 0)
                continue;
            renderSprite(board.openCells[i].back());
        }

        for (int i = 0; i < 4; i++)
        {
            if (board.foundations[i].size() == 0)
                continue;
            else if(board.foundations[i].size() > 1)
                renderSprite(board.foundations[i][board.foundations[i].size() - 2]);
            renderSprite(board.foundations[i].back());
        }

        drawCall();
    }

}

void Renderer::renderBackground(const Board& board, RenderMode mode)
{

    glEnable(GL_BLEND);

    if (mode == RenderMode::Shaded || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_unlitShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        for (int i = 0; i < 4; i++)
        {
            board.openCellsBg->pos = glm::vec3(board.openCellsMap[i], 0.0);
            renderSprite(board.openCellsBg);
        }

        for (int i = 0; i < 4; i++)
        {
            board.foundationsBg->pos = glm::vec3(board.foundationsMap[i], 0.0);
            renderSprite(board.foundationsBg);
        }

        drawCall();
    }

    if (mode == RenderMode::Wireframe || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_wireframeShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (int i = 0; i < 4; i++)
        {
            board.openCellsBg->pos = glm::vec3(board.openCellsMap[i], 0.0);
            renderSprite(board.openCellsBg);
        }

        for (int i = 0; i < 4; i++)
        {
            board.foundationsBg->pos = glm::vec3(board.foundationsMap[i], 0.0);
            renderSprite(board.foundationsBg);
        }

        drawCall();
    }
}

void Renderer::renderSprite(Card* card)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, card->pos);
    model = glm::scale(model, glm::vec3(76, 76, 1));

    m_shader->setMat4("u_model[" + std::to_string(m_instanceCounter) + "]", model);
    m_shader->setVec2("u_offset[" + std::to_string(m_instanceCounter) + "]", card->uvOffset.x, card->uvOffset.y);

    m_instanceCounter++;
}

void Renderer::drawCall()
{
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_model->indices.size(), GL_UNSIGNED_INT, nullptr, m_instanceCounter);
    glBindVertexArray(0);
}

void Renderer::initMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_model->vertices.size() * sizeof(Vertex), &m_model->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_model->indices.size() * sizeof(int), &m_model->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

}

void Renderer::clear()
{
    glClearColor(0.22f, 0.49f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
