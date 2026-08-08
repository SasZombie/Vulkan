#pragma once

#include "volk.h"
#include <vector>
#include <iostream>

namespace sas
{

    class VulkanInstanceWrapper;

    class VulkanPhysicalDevice
    {
    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    public:
        [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const noexcept
        {
            return physicalDevice;
        }

        operator VkPhysicalDevice() const noexcept
        {
            return physicalDevice;
        }

        VulkanPhysicalDevice(const VulkanInstanceWrapper &instanceWrapper);

        VulkanPhysicalDevice(const VulkanPhysicalDevice &) noexcept = default;
        VulkanPhysicalDevice &operator=(const VulkanPhysicalDevice &) noexcept = default;
        VulkanPhysicalDevice(VulkanPhysicalDevice &&) noexcept = default;
        VulkanPhysicalDevice &operator=(VulkanPhysicalDevice &&) noexcept = default;
        ~VulkanPhysicalDevice() = default;
    };

} // namespace sas
