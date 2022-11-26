#include "renderer.h"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "resource_manager.h"
#include "utils/log.h"
#include "window.h"
#include <vector>

void Renderer::init()
{
    initMesh();
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_proj = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f);
    m_unlitShader = ResourceManager::loadShader("../../resources/unlit.vert",
                                           "../../resources/unlit.frag", "Unlit");
    m_wireframeShader = ResourceManager::loadShader("../../resources/unlit.vert",
                                                    "../../resources/wireframe.frag",
                                                    "Wireframe");
    m_texture = ResourceManager::loadTexture("../../resources/cards.png", "cards");

    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    m_wireframeShader->use();
    m_wireframeShader->setMat4("u_proj", m_proj);

    m_unlitShader->use();
    m_unlitShader->setMat4("u_proj", m_proj);
    m_unlitShader->setInt("u_main_tex", 0);
}

void Renderer::render(const Board& board, RenderMode mode)
{
    clear();

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
                renderSprite(board.tableMap[j][i], board.table[j].at(i));
            }
        }

        for (int i = 0; i < 4; i++)
        {
            renderSprite(board.openCellsMap[i], board.openCells[i].back());
        }

        for (int i = 0; i < 4; i++)
        {
            renderSprite(board.foundationMap[i], board.foundations[i].back());
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
                renderSprite(board.tableMap[j][i], board.table[j].at(i));
            }
        }

        for (int i = 0; i < 4; i++)
        {
            renderSprite(board.openCellsMap[i], board.openCells[i].back());
        }

        for (int i = 0; i < 4; i++)
        {
            renderSprite(board.foundationMap[i], board.foundations[i].back());
        }

        drawCall();
    }

}

void Renderer::renderSprite(glm::vec2 pos, Card* card)
{
    glm::mat4 model = glm::mat4(1.0f);
    if (card->dragging)
    {
        glm::vec2 mousePos = glm::vec2(Window::xPos, Window::yPos);
        if (card->shouldSetOffset)
        {
            setDragOffset(pos);
            card->shouldSetOffset = false;
        }
        model = glm::translate(model, glm::vec3(mousePos - dragOffset, 0.0));
    }
    else
    {
        model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
    }
    model = glm::scale(model, glm::vec3(80, 80, 1));

    m_shader->setMat4("u_model[" + std::to_string(m_instanceCounter) + "]", model);
    m_shader->setVec2("u_offset[" + std::to_string(m_instanceCounter) + "]", card->offsetX, card->offsetY);
    m_shader->setVec3("u_tint[" + std::to_string(m_instanceCounter) + "]", card->selectionTint);

    m_instanceCounter++;
}


void Renderer::drawCall()
{
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, m_instanceCounter);
    glBindVertexArray(0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setDragOffset(glm::vec2 pos)
{
    glm::vec2 mousePos = glm::vec2(Window::xPos, Window::yPos);
    dragOffset = mousePos - pos;
}
