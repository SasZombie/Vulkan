#pragma once

#include <vector>

#include "Math.hpp"

namespace sas
{

    struct Vertex
    {
        math::Vec3 pos;
        math::Vec3 color;
        math::Vec2 texCoord;
    };

} // namespace sas