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
        VulkanAllocator(const VulkanInstanceWrapper &instance, const VulkanPhysicalDevice &physicalDev, const VulkanDevice &device);

        VulkanAllocator(const VulkanAllocator &) = delete;
        VulkanAllocator &operator=(const VulkanAllocator &) = delete;

        VulkanAllocator(VulkanAllocator &&other) noexcept
            : allocator(other.allocator)
        {
            other.allocator = nullptr;
        }

        VulkanAllocator &operator=(VulkanAllocator &&other) noexcept
        {
            if (this != &other)
            {
                if (allocator != VK_NULL_HANDLE)
                {
                    vmaDestroyAllocator(allocator);
                }
                allocator = other.allocator;
                other.allocator = VK_NULL_HANDLE;
            }
            return *this;
        }

        operator VmaAllocator() const noexcept
        {
            return allocator;
        }

        ~VulkanAllocator() noexcept
        {
            vmaDestroyAllocator(allocator);
        }
    };
} // namespace sas
