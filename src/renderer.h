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
    void initBackgroundMesh();
    void renderBackground(RenderMode mode);
    void renderCardBackground(const Board& board, RenderMode mode);
    void renderSprite(Card* card);
    void renderMovingSprite(glm::vec2 srcPos, glm::vec2 dstPos, float len, float startTime, Card* card);
    void drawCall();
    void clear();

    glm::mat4 m_proj;
    Shader* m_shader;
    Shader* m_unlitShader;
    Shader* m_wireframeShader;
    Texture* m_texture;
    Model* m_model;

    Model* m_backgroundModel;
    Shader* m_backgroundShader;
    Shader* m_backgroundWireframeShader;

    unsigned int m_instanceCounter;

    unsigned int VBO, EBO, VAO;
    unsigned int VBO_BG, EBO_BG, VAO_BG;
};
