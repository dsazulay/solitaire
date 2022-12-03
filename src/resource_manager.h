#pragma once

#include <map>
#include <string>

#include "shader.h"
#include "texture.h"
#include "model.h"

class ResourceManager
{
public:
    static Shader* loadShader(const char* vertShaderFile, const char* fragShaderFile, std::string name);
    static Texture* loadTexture(const char* textureFile, std::string name);
    static Model* loadModel(const char* modelFile, std::string name);


    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture> textures;
    static std::map<std::string, Model> models;
private:
    ResourceManager() {}
};
