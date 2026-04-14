#pragma once

#include <string>
#include <filesystem>
#include <cstdint>

class Shader
{
public:
    std::string filePath;
    std::filesystem::file_time_type lastWriteTime;

    size_t bufferSize;
    uint32_t* bufferPointer;
};
