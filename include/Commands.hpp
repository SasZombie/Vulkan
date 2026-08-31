#pragma once

#include <unordered_map>
#include <vector>
#include <span>
#include <string>

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

    struct CreateNewMeshCommand : ICommand
    {
        std::string meshPath;
        CreateNewMeshCommand(std::string path) : 
            meshPath(std::move(path))
        {}
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