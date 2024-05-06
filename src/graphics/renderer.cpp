#include "renderer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "resource_manager.h"

constexpr const char* unlitVertPath = "../../resources/unlit.vert";
constexpr const char* unlitFragPath = "../../resources/unlit.frag";
constexpr const char* bgVertPath = "../../resources/background.vert";
constexpr const char* bgFragPath = "../../resources/background.frag";
constexpr const char* wireframeFragPath = "../../resources/wireframe.frag";
constexpr const char* cardsTexPath = "../../resources/cards.png";
constexpr const char* cardModelPath = "../../resources/card.obj";

constexpr const float width = 1280.0f;
constexpr const float height = 720.0f;

auto Renderer::init() -> void
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_proj = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    m_unlitShader = ResourceManager::loadShader(
            unlitVertPath, unlitFragPath, "UnlitShader");
    m_wireframeShader = ResourceManager::loadShader(
            unlitVertPath, wireframeFragPath, "WireframeShader");
    m_backgroundShader = ResourceManager::loadShader(
            bgVertPath, bgFragPath, "BackgroundShader");
    m_backgroundWireframeShader = ResourceManager::loadShader(
            bgVertPath, wireframeFragPath, "BackgroundWireframeShader");
    m_texture = ResourceManager::loadTexture(cardsTexPath, "CardTex");

    m_model = ResourceManager::loadModel(cardModelPath, "CardModel");
    m_backgroundModel = ResourceManager::loadModel(
            NativeModel::Quad, "QuadModel");

    createUBO();

    setCameraUBO();
    setShaderUniformBlock();
    setShaderUniforms();
}

auto Renderer::render(std::span<CardEntity*> cards, std::span<CardBg*> cardsBg,
        RenderMode mode) -> void
{
    clear();

    if (mode == RenderMode::Shaded || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_unlitShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_BLEND);
        for (auto card : cardsBg)
        {
            renderSprite(card->sprite, card->transform.model());
        }
        drawCall();
        glDisable(GL_BLEND);

        m_instanceCounter = 0;
        for (auto card : cards)
        {
            renderSprite(card->sprite, card->transform.model());
        }
        drawCall();
    }

    if (mode == RenderMode::Wireframe || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_wireframeShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glEnable(GL_BLEND);
        for (auto card : cardsBg)
        {
            renderSprite(card->sprite, card->transform.model());
        }
        drawCall();
        glDisable(GL_BLEND);

        m_instanceCounter = 0;
        for (auto card : cards)
        {
            renderSprite(card->sprite, card->transform.model());
        }
        drawCall();
    }

    renderBackground(mode);
}

auto Renderer::reloadShaders() -> void
{
    setShaderUniforms();
}

auto Renderer::renderBackground(RenderMode mode) -> void
{
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

        glBindVertexArray(m_backgroundModel->mesh.getVao());
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

        glBindVertexArray(m_backgroundModel->mesh.getVao());
        glDrawElements(GL_TRIANGLES, m_backgroundModel->indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}

auto Renderer::renderSprite(Sprite sprite, const glm::mat4& model) -> void
{
    m_shader->setMat4("u_model[" + std::to_string(m_instanceCounter) + "]",
            model);
    m_shader->setVec2("u_offset[" + std::to_string(m_instanceCounter) + "]",
            sprite.uv.x, sprite.uv.y);

    m_instanceCounter++;
}

void Renderer::drawCall()
{
    glBindVertexArray(m_model->mesh.getVao());
    glDrawElementsInstanced(GL_TRIANGLES, m_model->indices.size(),
            GL_UNSIGNED_INT, nullptr, m_instanceCounter);
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
    m_unlitShader->setUniformBlock("Matrices", 0);
    m_wireframeShader->setUniformBlock("Matrices", 0);
    m_backgroundShader->setUniformBlock("Matrices", 0);
    m_backgroundWireframeShader->setUniformBlock("Matrices", 0);
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
