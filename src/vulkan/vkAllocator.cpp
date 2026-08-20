#include "vkAllocator.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

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