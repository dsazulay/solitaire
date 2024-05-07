#include "renderer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "resource_manager.h"

constexpr const char* UNLITVERTPATH = "../../resources/unlit.vert";
constexpr const char* UNLITFRAGPATH = "../../resources/unlit.frag";
constexpr const char* BGVERTPATH = "../../resources/background.vert";
constexpr const char* BGFRAGPATH = "../../resources/background.frag";
constexpr const char* WIREFRAMEFRAGPATH = "../../resources/wireframe.frag";
constexpr const char* CARDSTEXPATH = "../../resources/cards.png";
constexpr const char* CARDMODELPATH = "../../resources/card.obj";

constexpr const float WIDTH = 1280.0;
constexpr const float HEIGHT = 720.0;

constexpr const glm::vec3 CLEAR_COLOR{ 0.22, 0.49, 0.3 };
constexpr const glm::vec3 BG_POS{ 640.0, 360.0, -0.01 };
constexpr const glm::vec2 BG_SCALE{ WIDTH, HEIGHT };

auto Renderer::init() -> void
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
    m_unlitShader = ResourceManager::loadShader(
            UNLITVERTPATH, UNLITFRAGPATH, "UnlitShader");
    m_wireframeShader = ResourceManager::loadShader(
            UNLITVERTPATH, WIREFRAMEFRAGPATH, "WireframeShader");
    m_backgroundShader = ResourceManager::loadShader(
            BGVERTPATH, BGFRAGPATH, "BackgroundShader");
    m_backgroundWireframeShader = ResourceManager::loadShader(
            BGVERTPATH, WIREFRAMEFRAGPATH, "BackgroundWireframeShader");
    m_texture = ResourceManager::loadTexture(CARDSTEXPATH, "CardTex");

    m_model = ResourceManager::loadModel(CARDMODELPATH, "CardModel");
    m_backgroundModel = ResourceManager::loadModel(
            NativeModel::Quad, "QuadModel");

    m_backgroundTransform.pos(BG_POS);
    m_backgroundTransform.scale(BG_SCALE);

    createUBO();

    setCameraUBO();
    setShaderUniformBlock();
    setShaderUniforms();
}

auto Renderer::render(const std::span<CardEntity*> cards,
        const std::span<CardBg> cardBgs, RenderMode mode) -> void
{
    clear();

    if (mode == RenderMode::Shaded || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_unlitShader;
        m_instanceCounter = 0;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_BLEND);
        for (auto card : cardBgs)
        {
            renderSprite(card.sprite, card.transform.model());
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
        for (auto card : cardBgs)
        {
            renderSprite(card.sprite, card.transform.model());
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

        m_shader->setMat4("u_model", m_backgroundTransform.model());
        m_shader->setVec3("u_tint", CLEAR_COLOR);

        glBindVertexArray(m_backgroundModel->mesh.getVao());
        glDrawElements(GL_TRIANGLES, (int) m_backgroundModel->indices.size(),
                GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    if (mode == RenderMode::Wireframe || mode == RenderMode::ShadedWireframe)
    {
        m_shader = m_backgroundWireframeShader;
        m_shader->use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        m_shader->setMat4("u_model", m_backgroundTransform.model());
        m_shader->setVec3("u_tint", CLEAR_COLOR);

        glBindVertexArray(m_backgroundModel->mesh.getVao());
        glDrawElements(GL_TRIANGLES, (int) m_backgroundModel->indices.size(),
                GL_UNSIGNED_INT, nullptr);
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
    glDrawElementsInstanced(GL_TRIANGLES, (int) m_model->indices.size(),
            GL_UNSIGNED_INT, nullptr, (int) m_instanceCounter);
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
    glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
