#include "resource_manager.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "utils/log.h"

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::textures;
std::map<std::string, Model> ResourceManager::models;

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

Model* ResourceManager::loadModel(const char *modelFile, std::string name)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFile)) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, int> uniqueVertices{};
    Model model;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
                model.vertices.push_back(vertex);
            }

            model.indices.push_back(uniqueVertices[vertex]);
        }
    }

    models[name] = model;

    return &models[name];
}

Model* ResourceManager::loadModel(NativeModel type, std::string name)
{
    Model model;

    model.vertices.reserve(16);
    model.indices.reserve(6);

    for (int i = 0; i < 16; i += 4)
    {
        Vertex vertex{};
        vertex.pos = {
            m_vertices[i], m_vertices[i + 1], 0.0f
        };

        vertex.texCoord = {
            m_vertices[i + 2], m_vertices[i + 3]
        };

        model.vertices.emplace_back(vertex);
    }

    for (int i = 0; i < 6; i++)
    {
        model.indices.emplace_back(m_indices[i]);
    }

    models[name] = model;

    return &models[name];
}
