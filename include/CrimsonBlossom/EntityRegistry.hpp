#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <typeindex>

#include "ComponentContainer.hpp"
#include "CommandBus.hpp"

namespace sas
{

    template <typename... Types>
    struct Combined
    {
        uint32_t entity;
        std::tuple<Types *...> components;

        template <typename T>
        T *get() { return std::get<T *>(components); }

        template <typename T>
        const T *get() const { return std::get<T *>(components); }
    };

    class EntityRegistry
    {
    private:
        uint32_t nextId = 0;
        CommandBus &bus;
        std::unordered_map<std::type_index, std::unique_ptr<IComponent>> componentsMap;

        template <typename T>
        ComponentContainer<T> *getComponentContainer() noexcept
        {
            std::type_index typeInd = typeid(T);

            if (componentsMap.find(typeInd) == componentsMap.end())
            {
                componentsMap[typeInd] = std::make_unique<ComponentContainer<T>>();
            }

            return static_cast<ComponentContainer<T> *>(componentsMap[typeInd].get());
        }

    public:
        EntityRegistry(CommandBus &cbus) noexcept
            : bus(cbus)
        {
            bus.subscribe<SpawnEntityCommand>([this](const SpawnEntityCommand &cmd)
                                              {
                (void)cmd;
                createEntity(); });
        }

        // MoveCtor
        EntityRegistry(const EntityRegistry &other) noexcept
            : nextId(other.nextId), bus(other.bus)
        {
            for (const auto &[type, container] : other.componentsMap)
            {
                componentsMap[type] = container->copy();
            }
        }

        EntityRegistry &operator=(const EntityRegistry &other) noexcept
        {
            if (this != &other)
            {
                nextId = other.nextId;
                componentsMap.clear();
                for (const auto &[type, container] : other.componentsMap)
                {
                    componentsMap[type] = container->copy();
                }
            }
            return *this;
        }

        EntityRegistry(EntityRegistry &&) noexcept = default;
        EntityRegistry &operator=(EntityRegistry &&) noexcept = default;

        uint32_t createEntity() noexcept
        {
            return nextId++;
        }

        [[nodiscard]] uint32_t getEntityCount() const noexcept
        {
            return nextId;
        }

        void destroyEntity(uint32_t entityId) noexcept
        {
            for (auto &pair : componentsMap)
            {
                pair.second->entityDistroyed(entityId);
            }
        }

        template <typename T>
        void addComponent(uint32_t entityId, T component) noexcept
        {
            getComponentContainer<T>()->insert(entityId, component);
        }

        template <typename T>
        void removeComponent(uint32_t entity)
        {
            getComponentContainer<T>()->remove(entity);
        }

        template <typename T>
        ComponentContainer<T> *getComponents() noexcept
        {
            return getComponentContainer<T>();
        }

        template <typename Primary, typename... Rest>
        void addListQueeryHandler() noexcept
        {

            bus.subscribe<QuerryListCommand<Combined<Primary, Rest...>>>(
                [this](const QuerryListCommand<Combined<Primary, Rest...>> &cmd)
                {
                    cmd.objList = this->getCombined<Primary, Rest...>();
                });
        }

        template <typename Primary, typename... Rest>
        std::vector<Combined<Primary, Rest...>> getCombined() noexcept
        {
            std::vector<Combined<Primary, Rest...>> result;
            auto *primary = getComponentContainer<Primary>();

            const auto &entities = primary->getEntities();
            auto &data = primary->getData();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                uint32_t entityId = entities[i];
                bool hasAll = (... && (getComponentContainer<Rest>()->get(entityId) != nullptr));

                if (hasAll)
                {
                    result.push_back({entityId,
                                      std::make_tuple(
                                          &data[i],
                                          getComponentContainer<Rest>()->get(entityId)...)});
                }
            }
            return result;
        }
    };

    struct RenderObject;
    struct ObjectTransform3D;
    using DrawingComponents = sas::Combined<sas::RenderObject, sas::ObjectTransform3D>;

} // namespace sas
