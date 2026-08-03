#include "vkPhysicalDevices.hpp"

#include "vkInitialize.hpp"
#include <exception>

VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstanceWrapper& instanceWrapper)
{

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instanceWrapper, &deviceCount, nullptr);

    if(deviceCount == 0)
    {
        throw std::runtime_error("No devices were found!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instanceWrapper, &deviceCount, devices.data());

    for (const auto &device : devices)
    {
        
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.apiVersion < VK_API_VERSION_1_4)
        {
            continue;
        }

        VkPhysicalDeviceVulkan14Features features14{};
        features14.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;

        VkPhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = &features14;

        vkGetPhysicalDeviceFeatures2(device, &deviceFeatures2);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            physicalDevice = device;
            std::cout << "Selected GPU: " << properties.deviceName << " (Vulkan 1.4 Core)\n";
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE && !devices.empty())
    {
        physicalDevice = devices[0];
    }
}