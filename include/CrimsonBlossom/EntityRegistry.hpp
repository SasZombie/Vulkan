#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <typeindex>

#include "ComponentContainer.hpp"

namespace sas
{
    class EntityRegistry
    {
    private:
        uint32_t nextId = 0;
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
        EntityRegistry() = default;

        EntityRegistry(const EntityRegistry &other)
            : nextId(other.nextId)
        {
            for (const auto &[type, container] : other.componentsMap)
            {
                componentsMap[type] = container->copy();
            }
        }

        // Deep Copy Assignment
        EntityRegistry &operator=(const EntityRegistry &other)
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

        // Default Move Operations
        EntityRegistry(EntityRegistry &&) noexcept = default;
        EntityRegistry &operator=(EntityRegistry &&) noexcept = default;

        uint32_t createEntity() noexcept
        {
            return nextId++;
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
    };

} // namespace sas
