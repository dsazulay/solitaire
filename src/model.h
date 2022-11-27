#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <vector>

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && texCoord == other.texCoord;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

class Model
{
public:
    std::vector<Vertex> vertices;
    std::vector<int> indices;

};
