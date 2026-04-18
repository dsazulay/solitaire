#include "resource_manager.h"

#include <filesystem>
#include <slang/slang.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "../utils/log.h"

std::unordered_map<std::string, Shader> ResourceManager::shaders;
//std::map<std::string, Texture> ResourceManager::textures;
std::unordered_map<std::string, Model> ResourceManager::models;
Slang::ComPtr<slang::IGlobalSession> ResourceManager::m_slangGlobalSession;
Slang::ComPtr<slang::ISession> ResourceManager::m_slangSession;

auto ResourceManager::initShaderCompiler() -> void
{
    // Initialize Slang shader compiler
    slang::createGlobalSession(m_slangGlobalSession.writeRef());
    auto slangTargets{
        std::to_array<slang::TargetDesc>({{
            .format = SLANG_SPIRV,
            .profile = m_slangGlobalSession->findProfile("spirv_1_4")
        }})
    };
    auto slangOptions{
        std::to_array<slang::CompilerOptionEntry>({{
            slang::CompilerOptionName::EmitSpirvDirectly,
            { slang::CompilerOptionValueKind::Int, 1 }
        }})
    };
    slang::SessionDesc slangSessionDesc{
        .targets = slangTargets.data(),
        .targetCount = SlangInt(slangTargets.size()),
        .defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
        .compilerOptionEntries = slangOptions.data(),
        .compilerOptionEntryCount = uint32_t(slangOptions.size())
    };

    m_slangGlobalSession->createSession(slangSessionDesc, m_slangSession.writeRef());
}

auto ResourceManager::loadShader(const char* shaderFile, std::string name) -> Shader*
{
    if (m_slangSession == nullptr)
    {
        initShaderCompiler();
        LOG_INFO("Init slang session");
    }
    // Load shader
    Slang::ComPtr<slang::IModule> slangModule{
        m_slangSession->loadModuleFromSource(shaderFile, shaderFile, nullptr, nullptr)
    };
    Slang::ComPtr<ISlangBlob> spirv;
    slangModule->getTargetCode(0, spirv.writeRef());

    Shader& shader = shaders[name];
    shader.filePath = shaderFile;
    shader.lastWriteTime = std::filesystem::last_write_time(shaderFile);
    shader.bufferSize = spirv->getBufferSize();
    shader.bufferPointer = (uint32_t*) spirv->getBufferPointer();

    return &shader;
}

auto ResourceManager::recompileShaders() -> void
{
    for (const auto& [name, shader] : shaders)
    {
        if (shader.lastWriteTime != std::filesystem::last_write_time(shader.filePath))
        {
            loadShader(shader.filePath.c_str(), name);
            LOG_INFO("{} shader recompiled", name);
        }
    }
}
/*
auto ResourceManager::loadTexture(const char* textureFile, std::string name) -> Texture*
{
    stbi_set_flip_vertically_on_load(true);
    Texture texture{};

    int width{}, height{}, nrChannels{};
    unsigned char* data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    texture.generate(width, height, data);

    textures[name] = texture;

    stbi_image_free(data);
    return &textures[name];
}*/

auto ResourceManager::loadModel(const char* modelFile, std::string name) -> Model*
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
    Model* ptr = &models[name];

    return ptr;
}

auto ResourceManager::loadModel(NativeModel type, std::string name) -> Model*
{
    Model model;

    model.vertices.reserve(m_vertices.size());
    model.indices.reserve(m_indices.size());

    for (int i = 0; i < static_cast<int>(m_vertices.size()); i += 4)
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

    for (auto& index : m_indices)
    {
        model.indices.emplace_back(index);
    }

    models[name] = model;
    Model* ptr = &models[name];

    return ptr;
}
