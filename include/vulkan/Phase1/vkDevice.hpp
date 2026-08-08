#pragma once

#include "volk.h"
namespace sas
{
    

class VulkanPhysicalDevice;

class VulkanDevice
{
private:
    uint32_t graphicsQueueFamilyIndex;
    VkDevice device;
    VkQueue graphicsQueue;

public:
    VulkanDevice(const VulkanPhysicalDevice &physicalDevice);

    VulkanDevice(const VulkanDevice &) = delete;
    VulkanDevice &operator=(const VulkanDevice &) = delete;

    VulkanDevice(VulkanDevice &&other) noexcept : device(other.device)
    {
        other.device = VK_NULL_HANDLE;
    }

    VulkanDevice &operator=(VulkanDevice &&other) noexcept
    {
        if (this != &other)
        {
            if (device != VK_NULL_HANDLE)
            {
                vkDeviceWaitIdle(device);
                vkDestroyDevice(device, nullptr);
            }
            device = other.device;
            other.device = VK_NULL_HANDLE;
        }
        return *this;
    }

    ~VulkanDevice()
    {
        if (device != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(device);
            vkDestroyDevice(device, nullptr);
        }
    }

    [[nodiscard]] VkDevice getDevice() const noexcept
    {
        return device;
    }

    operator VkDevice() const noexcept 
    { 
        return device; 
    }

    [[nodiscard]] VkQueue getQueue() const noexcept
    {
        return graphicsQueue;
    }

    [[nodiscard]] uint32_t getGraphicsQueueFamilyIndex() const noexcept
    {
        return graphicsQueueFamilyIndex;
    }
};
} // namespace sas
