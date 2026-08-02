#include "vkBridge.hpp"

#include "vkDevice.hpp"
#include "vkInitialize.hpp"
#include "vkPhysicalDevices.hpp"
#include "window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VulkanBridge::VulkanBridge(const Window &window, VulkanInstanceWrapper &instance, const VulkanPhysicalDevice &physicalDevice, const VulkanDevice& device)
    : vulkanInstance(instance)
{
    if (glfwCreateWindowSurface(vulkanInstance, window.window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!\n");
    }

    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, device.getGraphicsQueueFamilyIndex(), surface, &presentSupport);

    if (!presentSupport)
    {
        throw std::runtime_error("Cannot support window surface!\n");
    }

    
}