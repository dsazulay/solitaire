#include "shader.h"
#include <glad/glad.h>

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setInt(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

//void Shader::setMat4(const std::string& name, const glm::mat4& mat)
//{
//    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str(), 1, GL_FALSE, &mat[0][0]);
//}
