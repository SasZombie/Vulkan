#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanInstanceWrapper
{
private:
    VkInstance instance;

public:
    VulkanInstanceWrapper();

    VulkanInstanceWrapper(const VulkanInstanceWrapper &) = delete;
    VulkanInstanceWrapper &operator=(const VulkanInstanceWrapper &) = delete;

    VulkanInstanceWrapper(VulkanInstanceWrapper &&other) noexcept
        : instance(other.instance)
    {
        other.instance = VK_NULL_HANDLE;
    }

    VulkanInstanceWrapper &operator=(VulkanInstanceWrapper &&other) noexcept
    {
        if (this != &other)
        {
            if (instance != VK_NULL_HANDLE)
            {
                vkDestroyInstance(instance, nullptr);
            }
            instance = other.instance;
            other.instance = VK_NULL_HANDLE;
        }
        return *this;
    }

    [[nodiscard]] VkInstance getInstance() const noexcept
    {
        return instance;
    }
    ~VulkanInstanceWrapper() noexcept
    {
        if (instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(instance, nullptr);
        }
    }

    operator VkInstance() const noexcept 
    { 
        return instance; 
    }
};
