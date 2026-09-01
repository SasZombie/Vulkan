#pragma once
#include <vector>

#include "ObjectService.hpp"
#include "EntityRegistry.hpp"

namespace sas
{

    class Scene
    {
    public:
        uint32_t id;
        CommandBus& bus;
        EntityRegistry sceneRegistry;
        ObjectService objService;

        Scene(uint32_t sceneId, CommandBus& comBus) noexcept
            : id(sceneId), bus(comBus), sceneRegistry(bus), objService(bus, sceneRegistry)
        {
            sceneRegistry.addListQueeryHandler<RenderObject, ObjectTransform3D>();
        }
    };

} // namespace sas
