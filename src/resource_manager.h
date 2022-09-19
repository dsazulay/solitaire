#pragma once

#include "shader.h"
#include "texture.h"

#include <map>
#include <string>

class ResourceManager
{
public:
    static Shader* loadShader(const char* vertShaderFile, const char* fragShaderFile, std::string name);
    static Texture* loadTexture(const char* textureFile, std::string name);

    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture> textures;
private:
    ResourceManager() {}
};
