#include "resource_manager.h"

#include "../utils/log.h"

#include <slang/slang.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>


std::unordered_map<std::string, Shader> ResourceManager::shaders;
//std::map<std::string, Texture> ResourceManager::textures;
std::unordered_map<std::string, Model> ResourceManager::models;
Slang::ComPtr<slang::IGlobalSession> ResourceManager::m_slangGlobalSession;
std::vector<slang::TargetDesc> ResourceManager::m_targets;
std::vector<slang::CompilerOptionEntry> ResourceManager::m_options;
slang::SessionDesc ResourceManager::m_slangSessionDesc;
bool ResourceManager::initialized = false;

auto ResourceManager::initShaderCompiler() -> void
{
    // Initialize Slang shader compiler
    slang::createGlobalSession(m_slangGlobalSession.writeRef());
    m_targets = {{
        .format = SLANG_SPIRV,
        .profile = m_slangGlobalSession->findProfile("spirv_1_4")
    }};
    m_options = {{
        slang::CompilerOptionName::EmitSpirvDirectly,
        { slang::CompilerOptionValueKind::Int, 1 }
    }};
    m_slangSessionDesc = slang::SessionDesc{
        .targets = m_targets.data(),
        .targetCount = SlangInt(m_targets.size()),
        .defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
        .compilerOptionEntries = m_options.data(),
        .compilerOptionEntryCount = uint32_t(m_options.size())
    };
}

template <typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(
        tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

auto ResourceManager::loadShader(const char* shaderFile, std::string name) -> Shader*
{
    if (!initialized)
    {
        initShaderCompiler();
        initialized = true;
        LOG_INFO("Init slang session");
    }

    Slang::ComPtr<slang::ISession> session;
    m_slangGlobalSession->createSession(m_slangSessionDesc, session.writeRef());

    // Load shader
    Slang::ComPtr<ISlangBlob> diagnostics;
    Slang::ComPtr<slang::IModule> slangModule{
        session->loadModuleFromSource(shaderFile, shaderFile, nullptr, diagnostics.writeRef())
    };

    if (diagnostics)
    {
        LOG_ERROR("Slang diagnostics: {}", (char*)diagnostics->getBufferPointer());
    }

    Slang::ComPtr<ISlangBlob> spirv;
    slangModule->getTargetCode(0, spirv.writeRef());

    Shader& shader = shaders[name];
    shader.filePath = shaderFile;
    shader.lastWriteTime = std::filesystem::last_write_time(shaderFile);
    shader.reloaded = false;
    shader.spirv = spirv;
    shader.bufferSize = spirv->getBufferSize();
    shader.bufferPointer = (uint32_t*) spirv->getBufferPointer();

    return &shader;
}

auto ResourceManager::recompileShaders() -> void
{
    for (auto& [name, shader] : shaders)
    {
        if (shader.lastWriteTime != std::filesystem::last_write_time(shader.filePath))
        {
            loadShader(shader.filePath.c_str(), name);
            shader.reloaded = true;
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
