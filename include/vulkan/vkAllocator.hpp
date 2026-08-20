#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"

#include "vk_mem_alloc.h"

#pragma GCC diagnostic pop

#include "vkVulkan.hpp"

namespace sas
{
    class VulkanAllocator
    {
    private:
        VmaAllocator allocator;

    public:
        VulkanAllocator(const VulkanInstanceWrapper &instance, const VulkanPhysicalDevice &physicalDev, const VulkanDevice &device);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags vmaFlags, VkBuffer& buffer, VmaAllocation& allocation, VmaAllocationInfo* allocInfo = nullptr) const noexcept;
        void createImage(const VkImageCreateInfo& imageInfo, VmaMemoryUsage usage, VkImage& image, VmaAllocation& allocation) const noexcept;
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
