#pragma once

#include <vector>
#include "mesh.h"

class Model
{
public:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    //Mesh mesh;
};
