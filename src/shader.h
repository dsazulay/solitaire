#pragma once
#include <string>
#include <glm/mat4x4.hpp>

class Shader
{
public:
    auto compile(const char* vertexSrc, const char* fragSrc) -> void;
    auto use() const -> void;
    auto setInt(const std::string& name, const int value) const -> void;
    auto setVec2(const std::string& name, const float x, const float y) const -> void;
    auto setVec3(const std::string& name, const glm::vec3& v) const -> void;
    auto setMat4(const std::string& name, const glm::mat4 &mat) const -> void;
private:
    unsigned int ID;

    constexpr static int logBufferSize = 512;
};
