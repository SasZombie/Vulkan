#pragma once
#include <unordered_map>
#include <vector>
#include <memory>

#include "Mesh.hpp"
#include "vkComponents.hpp"
#include "vkSampler.hpp"

namespace sas
{

    class AssetManager
    {
    private:
        struct PairHash
        {
            std::size_t operator()(const std::pair<std::string, std::string> &p) const noexcept
            {
                std::size_t h1 = std::hash<std::string>{}(p.first);
                std::size_t h2 = std::hash<std::string>{}(p.second);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        VulkanDevices &vulkanCtx;
        VulkanSharedObjects &sharedObjs;
        VulkanSampler sampler;
        std::unordered_map<std::string, RenderMesh> meshCache;
        std::unordered_map<std::string, RenderTexture> textureCache;
        std::unordered_map<std::pair<std::string, std::string>, VulkanDynamicShader, PairHash> shaderCache;

        Mesh getRawMesh(std::string_view path) const noexcept;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const noexcept;
        RenderMesh createGpuMesh(const sas::Mesh &mesh) const noexcept;

    public:
        AssetManager(VulkanDevices &ctx, VulkanSharedObjects &obj) noexcept;

        RenderMesh loadMesh(const std::string &path) noexcept;
        RenderTexture loadTexture(const std::string &path) noexcept;
        VulkanDynamicShader &loadShader(const std::string &vert = "", const std::string &frag = "") noexcept;

        void addTexture(RenderObject &objWithMesh, const std::string &path) noexcept;
        void addTexture(RenderObject &objWithMesh, const RenderTexture &texture) noexcept;

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
                    vkFreeMemory(vulkanCtx.vkDevice, seccond.vertexBufferMemory, nullptr);
                }
                if (seccond.indexBufferMemory)
                {
                    vkFreeMemory(vulkanCtx.vkDevice, seccond.indexBufferMemory, nullptr);
                }
            }

            for (auto &elem : textureCache)
            {
                const auto &seccond = elem.second;

                if (seccond.memory)
                {
                    vkFreeMemory(vulkanCtx.vkDevice, seccond.memory, nullptr);
                }
                if (seccond.image)
                {
                    vkDestroyImage(vulkanCtx.vkDevice, seccond.image, nullptr);
                }

                if (seccond.view)
                {
                    vkDestroyImageView(vulkanCtx.vkDevice, seccond.view, nullptr);
                }
            }
        }
    };

} // namespace sas