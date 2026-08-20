#include "vkAllocator.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmissing-declarations"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#pragma GCC diagnostic pop

sas::VulkanAllocator::VulkanAllocator(const VulkanInstanceWrapper &instance, const VulkanPhysicalDevice &physicalDev, const VulkanDevice &device)
{
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_4;
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.physicalDevice = physicalDev;
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    if (vmaCreateAllocator(&allocatorCreateInfo, &allocator) != VK_SUCCESS)
    {
        throw std::runtime_error("Cannot create allocator!");
    }
}

void sas::VulkanAllocator::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaAllocationCreateFlags vmaFlags, VkBuffer &buffer, VmaAllocation &allocation, VmaAllocationInfo *allocInfo) const noexcept
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    vmaAllocInfo.flags = vmaFlags;

    vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &buffer, &allocation, allocInfo);
}

void sas::VulkanAllocator::createImage(const VkImageCreateInfo &imageInfo, VmaMemoryUsage usage, VkImage &image, VmaAllocation &allocation) const noexcept
{
    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = usage;

    vmaCreateImage(allocator, &imageInfo, &vmaAllocInfo, &image, &allocation, nullptr);
}
