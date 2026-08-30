#pragma once
#include <unordered_map>
#include <vector>
#include <memory>

#include "Mesh.hpp"
#include "vkComponents.hpp"
#include "vkSampler.hpp"
#include "Logger.hpp"
#include "Material.hpp"

#include "CommandBus.hpp"

namespace sas
{

    class AssetManager
    {
    private:
        VulkanDevices &vulkanCtx;
        VulkanSharedObjects &sharedObjs;
        std::unordered_map<std::string, RenderMesh> meshCache;
    public:
        MaterialManager materialManager;
    private:

        Logger* logger = BaseLogger::getLogger("Asset");

        Mesh getRawMesh(std::string_view path) const noexcept;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const noexcept;
        RenderMesh createGpuMesh(const sas::Mesh &mesh) const noexcept;


    public:
        AssetManager(VulkanDevices &ctx, VulkanSharedObjects &obj) noexcept;
        RenderMesh loadMesh(const std::string &path) noexcept;
       

        ~AssetManager() noexcept
        {
            vkDeviceWaitIdle(vulkanCtx.vkDevice);
            for (auto &elem : meshCache)
            {
                const auto &seccond = elem.second;

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
                    vmaFreeMemory(sharedObjs.allocator, seccond.vertexBufferMemory);
                }
                if (seccond.indexBufferMemory)
                {
                    vmaFreeMemory(sharedObjs.allocator, seccond.indexBufferMemory);
                }
            }
        }
    };

} // namespace sas