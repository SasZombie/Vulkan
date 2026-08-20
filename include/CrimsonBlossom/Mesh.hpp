#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include "Math.hpp"

namespace sas
{

    //Cpu stuff
    struct Vertex
    {
        math::Vec3 pos;
        math::Vec3 normals;
        math::Vec2 texCoord;
        math::Vec3 color;
    };

    struct Texture
    {
        std::string path;
    };

    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<int> indices;
    };

} // namespace sas