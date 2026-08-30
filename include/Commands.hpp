#pragma once

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
} // namespace sas