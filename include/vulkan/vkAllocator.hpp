#pragma once

#include "vk_mem_alloc.h"
#include "vkVulkan.hpp"

namespace sas
{
    class VulkanAllocator
    {
    private:
        VmaAllocator allocator;

    public:
        VulkanAllocator(const VulkanInstanceWrapper& instance, const VulkanPhysicalDevice& physicalDev, const VulkanDevice& device);

        ~VulkanAllocator() noexcept
        {
            vmaDestroyAllocator(allocator);
        }
    };
} // namespace sas
