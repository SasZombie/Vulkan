#pragma once

#include "vkInitialize.hpp"
#include <vulkan/vulkan.h>

class Window;
class VulkanPhysicalDevice;
class VulkanInstanceWrapper;

class VulkanBridge
{
private:
    VulkanInstanceWrapper &vulkanInstance;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

public:
    VulkanBridge(const Window &window, VulkanInstanceWrapper &instance, const VulkanPhysicalDevice &physicalDevice, const VulkanDevice &device);

    ~VulkanBridge()
    {
        if (surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
            surface = VK_NULL_HANDLE;
        }
    }

    VulkanBridge(const VulkanBridge &) = delete;
    VulkanBridge &operator=(const VulkanBridge &) = delete;

    VulkanBridge(VulkanBridge &&other) noexcept : vulkanInstance(other.vulkanInstance)
    {
        other.surface = VK_NULL_HANDLE;
    }

    VulkanBridge &operator=(VulkanBridge &&other) noexcept
    {
        if (this != &other)
        {
            if (surface != VK_NULL_HANDLE)
            {
                vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
            }
            surface = other.surface;
            other.surface = VK_NULL_HANDLE;
        }
        return *this;
    }
};