#pragma once

#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <string>
#include <filesystem>
#include <cstdint>

class Shader
{
public:
    std::string filePath;
    std::filesystem::file_time_type lastWriteTime;
    bool reloaded;

    Slang::ComPtr<ISlangBlob> spirv;
    size_t bufferSize;
    uint32_t* bufferPointer;
};
