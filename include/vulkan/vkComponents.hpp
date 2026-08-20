#pragma once

#include "vkVulkan.hpp"
#include "vkAllocator.hpp"
#include "Math.hpp"

namespace sas
{

    struct PushConstants
    {
        math::Mat4 mvp;
    };
  

    class VulkanDevices
    {
    public:
        VulkanInstanceWrapper vk;
        VulkanPhysicalDevice vkPhysical;
        VulkanDevice vkDevice;
        VulkanBridge vkBridge;
        VulkanCommand vkCommand;

        VulkanDevices(const Window &window) noexcept
            : vk(),
              vkPhysical(vk),
              vkDevice(vkPhysical),
              vkBridge(window, vk, vkPhysical, vkDevice),
              vkCommand(vkDevice)
        {
        }
    };

    struct VulkanSharedObjects
    {
        VulkanDescriptor shaderDescriptor;
        VulkanAllocator allocator;

        VulkanSharedObjects(VulkanDevices& dev ) noexcept
            : shaderDescriptor(dev.vkDevice), allocator(dev.vk, dev.vkPhysical, dev.vkDevice)
        {
        }
    };

    struct VulkanPipelineComponents
    {
        VulkanBridge *bridge;
        VulkanInputPipeline *inputPipeline;
        VulkanViewport *viewport;
        VulkanRastarization *rastar;
        VulkanPixelStyling *pixelStyling;
        VulkanSharedObjects *sharedObjects;
    };

    struct RenderTexture
    {
        VkImage image;
        VkImageView view;
        VmaAllocation allocation;
    };

    struct RenderMesh
    {
        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
        VmaAllocation vertexBufferMemory;
        VmaAllocation indexBufferMemory;
        size_t indexCount;
    };

    struct RenderObject
    {
        RenderMesh *mesh;
        VulkanDynamicShader *shader;

        VkDescriptorSet descriptorSet;
    };

} // namespace sas
