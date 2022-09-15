#pragma once

#include "shader.h"

#include <map>
#include <string>

class ResourceManager
{
public:
    static Shader* loadShader(const char* vertShaderFile, const char* fragShaderFile, std::string name);

    static std::map<std::string, Shader> shaders;
private:
    ResourceManager() {}
};
