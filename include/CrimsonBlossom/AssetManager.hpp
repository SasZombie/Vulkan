#pragma once
#include <unordered_map>
#include <vector>
#include <memory>

#include "Mesh.hpp"
#include "vkComponents.hpp"
#include "vkSampler.hpp"
#include "Logger.hpp"
#include "Material.hpp"
#include "MaterialManager.hpp"

namespace sas
{

    class AssetManager
    {
    private:
        VulkanDevices &vulkanCtx;
        VulkanSharedObjects &sharedObjs;
    public:
        MaterialManager materialManager;
    private:
        CommandBus& bus;
        std::unordered_map<std::string, RenderMesh> gpuMeshCache;
        std::unordered_map<std::string, Mesh> cpuMeshCache;

        Logger* logger = BaseLogger::getLogger("Asset");

        Mesh getRawMesh(std::string_view path) noexcept;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const noexcept;
        RenderMesh createGpuMesh(const sas::Mesh &mesh) const noexcept;


    public:
        AssetManager(VulkanDevices &ctx, VulkanSharedObjects &obj, CommandBus& cmdBus) noexcept;
        RenderMesh loadMesh(const std::string &path) noexcept;
       

        ~AssetManager() noexcept
        {
            vkDeviceWaitIdle(vulkanCtx.vkDevice);
            for (auto &elem : gpuMeshCache)
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