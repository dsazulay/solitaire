#include "renderer.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
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
    m_backgroundShader = ResourceManager::loadShader("../../resources/background.vert",
                                                    "../../resources/background.frag",
                                                    "BackgroundShader");
    m_backgroundWireframeShader = ResourceManager::loadShader("../../resources/background.vert",
                                                    "../../resources/wireframe.frag",
                                                    "BackgroundWireframeShader");
    m_texture = ResourceManager::loadTexture("../../resources/cards.png", "CardTex");

    m_model = ResourceManager::loadModel("../../resources/card.obj", "CardModel");
    m_backgroundModel = ResourceManager::loadModel(NativeModel::Quad, "QuadModel");

    m_shaders.reserve(4);
    m_shaders.push_back(m_unlitShader);
    m_shaders.push_back(m_wireframeShader);
    m_shaders.push_back(m_backgroundShader);
    m_shaders.push_back(m_backgroundWireframeShader);

    initMesh();
    initBackgroundMesh();

    createUBO();

    setCameraUBO();
    setShaderUniformBlock();
    setShaderUniforms();
}

auto Renderer::render(const Board& board, RenderMode mode) -> void
{
    clear();

    renderCardBackground(board, mode);

    glDisable(GL_BLEND);

    if (mode == RenderMode::Shaded || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_unlitShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        for (int j = 0; j < 8; j ++)
        {
            for (int i = 0; i < (int) board.tableau[j].size(); i++)
            {
                renderSprite(board.tableau[j].at(i));
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
            for (int i = 0; i < (int) board.tableau[j].size(); i++)
            {
                renderSprite(board.tableau[j].at(i));
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

    renderBackground(mode);
}

auto Renderer::renderCardBackground(const Board& board, RenderMode mode) -> void
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
            renderSprite(board.openCellsBg[i]);
            renderSprite(board.foundationsBg[i]);
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
            renderSprite(board.openCellsBg[i]);
            renderSprite(board.foundationsBg[i]);
        }

        drawCall();
    }
}

auto Renderer::renderBackground(RenderMode mode) -> void
{

    glEnable(GL_BLEND);

    if (mode == RenderMode::Shaded || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_backgroundShader;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(640.0f, 360.0f, -0.01f));
        model = glm::scale(model, glm::vec3(1280, 720, 1));

        m_shader->setMat4("u_model", model);
        m_shader->setVec3("u_tint", glm::vec3(0.22f, 0.49f, 0.3f));

        glBindVertexArray(VAO_BG);
        glDrawElements(GL_TRIANGLES, m_backgroundModel->indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    if (mode == RenderMode::Wireframe || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_backgroundWireframeShader;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(640.0f, 360.0f, -0.01f));
        model = glm::scale(model, glm::vec3(1280, 720, 1));

        m_shader->setMat4("u_model", model);
        m_shader->setVec3("u_tint", glm::vec3(0.22f, 0.49f, 0.3f));

        glBindVertexArray(VAO_BG);
        glDrawElements(GL_TRIANGLES, m_backgroundModel->indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}

auto Renderer::renderSprite(Card* card) -> void
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

// TODO: create a new class for mesh objects to hold VAO EBO and VBO
auto Renderer::initMesh() -> void
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

    glBindVertexArray(0);
}

auto Renderer::initBackgroundMesh() -> void
{
    glGenVertexArrays(1, &VAO_BG);
    glGenBuffers(1, &VBO_BG);
    glGenBuffers(1, &EBO_BG);

    glBindVertexArray(VAO_BG);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_BG);
    glBufferData(GL_ARRAY_BUFFER, m_backgroundModel->vertices.size() * sizeof(Vertex), &m_backgroundModel->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_BG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_backgroundModel->indices.size() * sizeof(int), &m_backgroundModel->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

    glBindVertexArray(0);
}

auto Renderer::createUBO() -> void
{
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(m_proj), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

auto Renderer::setCameraUBO() -> void
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_proj), glm::value_ptr(m_proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

auto Renderer::setShaderUniformBlock() -> void
{
    for (auto shader : m_shaders)
    {
        shader->setUniformBlock("Matrices", 0);
    }
}

auto Renderer::setShaderUniforms() -> void
{
    glActiveTexture(GL_TEXTURE0);
    m_texture->bind();

    m_unlitShader->use();
    m_unlitShader->setInt("umain_tex", 0);
}

auto Renderer::clear() -> void
{
    glClearColor(0.22f, 0.49f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
