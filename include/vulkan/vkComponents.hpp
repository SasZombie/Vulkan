#pragma once

#include "vkBridge.hpp"
#include "vkShaderPipeline.hpp"
#include "vkInputPipeline.hpp"
#include "vkViewport.hpp"
#include "vkRastarization.hpp"
#include "vkPixelStyling.hpp"
#include "vkPhysicalDevices.hpp"
#include "vkCommand.hpp"
#include "vkDescriptor.hpp"

#include "Math.hpp"

namespace sas
{

    struct PushConstants
    {
        math::Mat4 mvp;
    };

    struct VulkanSharedObjects
    {
        VulkanDescriptor shaderDescriptor;

        VulkanSharedObjects(VulkanDevice &dev) noexcept
            : shaderDescriptor(dev)
        {
        }
    };

    struct VulkanPipelineComponents
    {
        VulkanBridge *bridge;
        VulkanShaderPipeline *shaderPipeline;
        VulkanInputPipeline *inputPipeline;
        VulkanViewport *viewport;
        VulkanRastarization *rastar;
        VulkanPixelStyling *pixelStyling;
        VulkanSharedObjects *sharedObjects;
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

    struct RenderTexture
    {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    };

    struct RenderObject
    {
        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkDeviceMemory indexBufferMemory;

        VulkanDynamicShaderPipeline *shader;

        VkDescriptorSet descriptorSet;

        size_t indexCount;
    };

} // namespace sas
