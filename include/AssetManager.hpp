#pragma once
#include <unordered_map>
#include <vector>
#include <memory>

#include "Mesh.hpp"
#include "vkComponents.hpp"

namespace sas
{
    class AssetManager
    {
    private:
        VulkanLowLevel& vulkanCtx;

    public:
        AssetManager(VulkanLowLevel &ctx)
            : vulkanCtx(ctx)
        {
        }
        RenderObject loadMesh(const std::string &path) noexcept;
    };

} // namespace sas