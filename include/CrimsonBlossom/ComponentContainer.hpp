#pragma once
#include <cstdint>
#include <vector>
#include <limits>

namespace sas
{

    constexpr uint32_t nullIndex = std::numeric_limits<uint32_t>::max();

    struct IComponent
    {
        virtual ~IComponent() noexcept = default;
        virtual void entityDistroyed(uint32_t entityId) noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<IComponent> copy() const noexcept = 0;
    };

    template <typename T>
    class ComponentContainer : public IComponent
    {
    private:
        std::vector<T> data;
        std::vector<uint32_t> entityList;
        std::vector<uint32_t> sparseVector;

    public:
        void insert(uint32_t entityId, T component) noexcept
        {
            if (entityId >= sparseVector.size())
            {
                sparseVector.resize(entityId + 1, nullIndex);
            }

            if (sparseVector[entityId] != nullIndex)
            {
                data[sparseVector[entityId]] = component;
                return;
            }

            uint32_t newIndex = data.size();
            data.push_back(component);
            entityList.push_back(entityId);
            sparseVector[entityId] = newIndex;
        }

        [[nodiscard]] T *get(uint32_t entityId) noexcept
        {
            if (entityId < sparseVector.size() && sparseVector[entityId] != nullIndex)
            {
                return &data[sparseVector[entityId]];
            }

            return nullptr;
        }

        [[nodiscard]] std::unique_ptr<IComponent> copy() const noexcept override 
        {
            return std::make_unique<ComponentContainer<T>>(*this);
        }

        void remove(uint32_t entityId) noexcept
        {
            if (entityId >= sparseVector.size() || sparseVector[entityId] == nullIndex)
            {
                return;
            }

            size_t indexToRemove = sparseVector[entityId];
            size_t lastElem = data.size() - 1;

            if (indexToRemove != lastElem)
            {
                data[indexToRemove] = data[lastElem];

                uint32_t lastEntity = entityList[lastElem];
                entityList[indexToRemove] = lastEntity;
                sparseVector[lastEntity] = indexToRemove;
            }

            data.pop_back();
            entityList.pop_back();
            sparseVector[entityId] = nullIndex;
        }
        void entityDistroyed(uint32_t entityId) noexcept override
        {
            remove(entityId);
        }

        std::vector<T> &getData()
        {
            return data;
        }
        std::vector<uint32_t> &getEntities()
        {
            return entityList;
        }
    };

} // namespace sas
