#pragma once
#include <vector>

#include "EntityRegistry.hpp"

namespace sas
{

    class Scene
    {
    public:
        uint32_t id;
        EntityRegistry sceneRegistry;

        Scene(uint32_t sceneId, CommandBus& bus) noexcept
            : id(sceneId), sceneRegistry(bus)
        {
            sceneRegistry.addListQueeryHandler<RenderObject, ObjectTransform3D>();
        }
    };

} // namespace sas
