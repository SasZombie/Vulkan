#pragma once
#include "Mesh.hpp"
#include "Material.hpp"

namespace sas
{    
    struct RenderObject
    {
        RenderMesh *mesh;
        Material *material;
    };
} // namespace sas