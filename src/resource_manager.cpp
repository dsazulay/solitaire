#include "resource_manager.h"

#include "utils/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <sstream>

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::textures;

Shader* ResourceManager::loadShader(const char *vertShaderFile, const char *fragShaderFile, std::string name)
{
    std::string vertexCode;
    std::string fragCode;

    try
    {
        std::ifstream vertexFile;
        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        vertexFile.open(vertShaderFile);

        std::ifstream fragFile;
        fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragFile.open(fragShaderFile);

        std::stringstream vertexStream, fragStream;
        vertexStream << vertexFile.rdbuf();
        fragStream << fragFile.rdbuf();

        vertexFile.close();
        fragFile.close();

        vertexCode = vertexStream.str();
        fragCode = fragStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        LOG_ERROR("Shader file not successfully read!");
    }

    Shader shader;
    shader.compile(vertexCode.c_str(), fragCode.c_str());
    shaders[name] = shader;

    return &shaders[name];
}

Texture* ResourceManager::loadTexture(const char* textureFile, std::string name)
{
    stbi_set_flip_vertically_on_load(true);
    Texture texture;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    texture.generate(width, height, data);

    textures[name] = texture;

    stbi_image_free(data);
    return &textures[name];
}
