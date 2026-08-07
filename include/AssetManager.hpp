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
        VulkanLowLevel &vulkanCtx;
        std::unordered_map<std::string, RenderObject> meshCache;

        Mesh getRawMesh(std::string_view path) const noexcept;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const noexcept;
        RenderObject createGpuMesh(const sas::Mesh &mesh) const noexcept;

    public:
        AssetManager(VulkanLowLevel &ctx) noexcept
            : vulkanCtx(ctx)
        {
        }
        RenderObject loadMesh(const std::string &path) noexcept;

        ~AssetManager() noexcept
        {
            vkDeviceWaitIdle(vulkanCtx.vkDevice);
            for (auto &elem : meshCache)
            {
                const auto& seccond = elem.second;


                if (seccond.vertexBuffer)
                {
                    vkDestroyBuffer(vulkanCtx.vkDevice, seccond.vertexBuffer, nullptr);
                }
                if (seccond.indexBuffer)
                {
                    vkDestroyBuffer(vulkanCtx.vkDevice, seccond.indexBuffer, nullptr);
                }

                if (seccond.vertexBufferMemory)
                {
                    vkFreeMemory(vulkanCtx.vkDevice, seccond.vertexBufferMemory, nullptr);
                }
                if (seccond.indexBufferMemory)
                {
                    vkFreeMemory(vulkanCtx.vkDevice, seccond.indexBufferMemory, nullptr);
                }
            }
        }
    };

} // namespace sas