#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;

    auto operator==(const Vertex& other) const -> bool
    {
        return pos == other.pos && texCoord == other.texCoord;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        auto operator()(Vertex const& vertex) const -> size_t
        {
            return hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

class Mesh
{
public:
    auto init(Vertex* vertices, long vSize, int* indices, long iSize) -> void;
    auto getVao() -> unsigned int;
private:
    unsigned int m_vao;
    unsigned int m_ebo;
    unsigned int m_vbo;
};
