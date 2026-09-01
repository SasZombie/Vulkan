#pragma once
#include "Logger.hpp"
#include "CommandBus.hpp"
#include "RenderObject.hpp"
#include "EntityRegistry.hpp"
#include "ObjectTransform.hpp"

namespace sas
{
    class ObjectService
    {
    private:
        CommandBus &bus;
        EntityRegistry &registry;
        Logger *logger = BaseLogger::getLogger("ObjectService");

    public:
        ObjectService(CommandBus &comBus, EntityRegistry &currentRegistry) noexcept
            : bus(comBus), registry(currentRegistry)
        {
            bus.subscribe<CreateNewEntityCommand>([this](const CreateNewEntityCommand &cmd)
                                                    { buildRendableEntity(cmd); });
        }

        void buildRendableEntity(const CreateNewEntityCommand &cmd) noexcept
        {
            logger->log("Creating new entity");
            const uint32_t newEntity = registry.createEntity();
            CreateRenderMeshCommand newMesh{cmd.mesh};
            CreateRenderMaterialCommand newMaterial{cmd.material};

            bus.dispatch(newMesh);
            bus.dispatch(newMaterial);

            RenderObject renObj;
            renObj.mesh = newMesh.renderMesh;
            renObj.material = newMaterial.renderMaterial;

            registry.addComponent(newEntity, renObj);
            registry.addComponent(newEntity, ObjectTransform3D{});
        }

        ~ObjectService() noexcept = default;
    };

} // namespace sas