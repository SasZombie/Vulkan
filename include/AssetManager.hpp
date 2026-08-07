#pragma once
#include <unordered_map>
#include <vector>
#include <memory>

#include "Vertex.hpp"

namespace sas
{
    class AssetManager
    {
    private:
        struct Slot
        {
            Mesh mesh;
            uint32_t generation = 0; 
            bool active = false;

            uint32_t nextFreeIndex = 0;
        };
        
        std::vector<Slot> slots;
        uint32_t firstFreeIndex = 0;
        uint32_t activeCount = 0;

    public:

        const Mesh* getMesh(const MeshComponent& component) const noexcept 
        {
            if(isValid(component))
            {
                return &slots[component.id].mesh;
            }

            return nullptr;
        }

        MeshComponent loadMesh(const std::string& path) noexcept;
        
        void unloadMesh(const MeshComponent& comp) noexcept;

        bool isValid(const MeshComponent& handle) const noexcept;

    };

} // namespace sas