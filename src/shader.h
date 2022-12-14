#pragma once
#include <string>
#include <glm/mat4x4.hpp>

class Shader
{
public:
    void compile(const char* vertexSrc, const char* fragSrc);
    void use() const;
    void setInt(const std::string& name, const int value) const;
    void setVec2(const std::string& name, const float x, const float y) const;
    void setVec3(const std::string& name, const glm::vec3& v) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
private:
    unsigned int ID;
    //unsigned int transformUBI;
};
