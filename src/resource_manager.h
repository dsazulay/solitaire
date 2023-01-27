#pragma once

#include <map>
#include <string>

#include "shader.h"
#include "texture.h"
#include "model.h"

enum class NativeModel
{
    Quad
};

class ResourceManager
{
public:
    static Shader* loadShader(const char* vertShaderFile, const char* fragShaderFile, std::string name);
    static Texture* loadTexture(const char* textureFile, std::string name);
    static Model* loadModel(const char* modelFile, std::string name);
    static Model* loadModel(NativeModel type, std::string name);


    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture> textures;
    static std::map<std::string, Model> models;
private:
    ResourceManager() {}

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
