#pragma once

#include <unordered_map>
#include <array>
#include <string>

#include "shader.h"
//#include "texture.h"
#include "model.h"

#include <slang/slang-com-ptr.h>

enum class NativeModel
{
    Quad
};

class ResourceManager
{
public:
    static auto loadShader(const char* shaderFile, std::string name) -> Shader*;
    static auto recompileShaders() -> void;
    //static auto loadTexture(const char* textureFile, std::string name) -> Texture*;
    static auto loadModel(const char* modelFile, std::string name) -> Model*;
    static auto loadModel(NativeModel type, std::string name) -> Model*;


    static std::unordered_map<std::string, Shader> shaders;
    //static std::map<std::string, Texture> textures;
    static std::unordered_map<std::string, Model> models;

private:
    static auto initShaderCompiler() -> void;

    static Slang::ComPtr<slang::IGlobalSession> m_slangGlobalSession;
    static Slang::ComPtr<slang::ISession> m_slangSession;

    static constexpr std::array<float, 16> m_vertices{
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    static constexpr std::array<int, 6> m_indices{
        0, 1, 2, 2, 3, 0
    };
};
