#pragma once

#include "vkVulkan.hpp"
#include "vkAllocator.hpp"
#include "vkSampler.hpp"
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
        VulkanDescriptor vulkanDescriptor;
        VulkanSampler sampler;
        VulkanAllocator allocator;

        VulkanSharedObjects(VulkanDevices& dev ) noexcept
            : vulkanDescriptor(dev.vkDevice), sampler(dev.vkDevice), allocator(dev.vk, dev.vkPhysical, dev.vkDevice)
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

} // namespace sas
