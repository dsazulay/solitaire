#include "shader.h"

#include "glm/fwd.hpp"
#include "utils/log.h"

#include <glad/glad.h>

auto Shader::compile(const char *vertexSrc, const char* fragSrc) -> void
{
    unsigned int vertexID{}, fragID{};
    int success{};
    std::string infoLog{};


    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexSrc, nullptr);
    glCompileShader(vertexID);

    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexID, Shader::logBufferSize, nullptr, infoLog.data());
        LOG_ERROR("Vertex shader compilation failed: {}", infoLog);
    }

    fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragID, 1, &fragSrc, nullptr);
    glCompileShader(fragID);

    glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragID, Shader::logBufferSize, nullptr, infoLog.data());
        LOG_ERROR("Fragment shader compilation failed: {}", infoLog);
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragID);

    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, Shader::logBufferSize, nullptr, infoLog.data());
        LOG_ERROR("Shader program linking failed: {}", infoLog);
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragID);
}

auto Shader::use() const -> void
{
    glUseProgram(ID);
}

auto Shader::setUniformBlock(const char* blockName, int blockID) -> void
{
    glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, blockName), blockID);
}

auto Shader::setInt(const std::string& name, const int value) const -> void
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

auto Shader::setVec2(const std::string& name, const float x, const float y) const -> void
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

auto Shader::setVec3(const std::string& name, const glm::vec3& v) const -> void
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &v[0]);
}

auto Shader::setMat4(const std::string& name, const glm::mat4& mat) const -> void
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
