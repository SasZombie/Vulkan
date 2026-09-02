#pragma once

#include <unordered_map>
#include <vector>
#include <span>
#include <string>

#include "InspectableComponents.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace sas
{
    struct ICommand
    {
        virtual ~ICommand() = default;
    };

    struct SpawnEntityCommand : ICommand
    {
        uint32_t targetSceneId;
    };

    struct DeleteEntity : ICommand
    {
        uint32_t id;
    };

    struct ItemSelected : ICommand
    {
        uint32_t id;
        ItemSelected(uint32_t newId) noexcept
            : id(newId)
        {
        }
    };

    struct CreateNewMeshCommand : ICommand
    {
        std::string meshPath;
        CreateNewMeshCommand(std::string path) noexcept
            : meshPath(std::move(path))
        {
        }
    };

    struct GetFormatInfoCommand : ICommand
    {
        uint32_t formatType;
    };

    struct SaveCommand : ICommand
    {
        //Could add path
    };

    struct GetMaterialByNameCommand : ICommand
    {
    };

    struct GetMeshByNameCommand : ICommand
    {
    };

    struct CreateRenderMeshCommand : ICommand
    {
        std::string renderMeshPath;
        mutable RenderMesh* renderMesh;

        CreateRenderMeshCommand(std::string rmPath) noexcept
            : renderMeshPath(rmPath)
        {
        }
    };

    struct CreateRenderMaterialCommand : ICommand
    {
        std::string renderMaterialPath;
        mutable Material* renderMaterial;

        CreateRenderMaterialCommand(std::string renderMatPath) noexcept
            : renderMaterialPath(renderMatPath)
        {
        }
    };

    struct CreateNewEntityCommand : ICommand
    {
        std::string mesh;
        std::string material;

        CreateNewEntityCommand(std::string nmesh, std::string nmaterial) noexcept
            : mesh(std::move(nmesh)), material(std::move(nmaterial))
        {
        }
    };

    struct QuerryEntityComponentsCommand : ICommand
    {
        uint32_t entityId;
        mutable std::vector<InspectableComponent> components;

        QuerryEntityComponentsCommand(uint32_t id) noexcept
            : entityId(id)
        {
        }
    };

    template <typename T>
    struct QuerrySpanCommand : ICommand
    {
        mutable std::span<T> objList;
    };

    template <typename T>
    struct QuerryListCommand : ICommand
    {
        mutable std::vector<T> objList;
    };

    template <typename K, typename V>
    struct QuerryMapCommand : ICommand
    {
        mutable const std::unordered_map<K, V> *map;
    };

} // namespace sas