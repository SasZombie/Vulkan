#pragma once

#include "vkInitialize.hpp"
#include "vkDevice.hpp"
#include <vector>
#include "volk.h"

namespace sas
{

    class Window;
    class VulkanPhysicalDevice;

    class VulkanBridge
    {
    private:
        VkSurfaceFormatKHR surfaceFormat;

        VulkanInstanceWrapper &vulkanInstance;
        VulkanDevice &vulkanDevice;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

    public:
        VulkanBridge(const Window &window, VulkanInstanceWrapper &instance, const VulkanPhysicalDevice &physicalDevice, VulkanDevice &device);

        [[nodiscard]] VkSurfaceFormatKHR getSurfaceFormat() const noexcept
        {
            return surfaceFormat;
        }

        [[nodiscard]] const VkSwapchainKHR &getSwapChain() const noexcept
        {
            return swapchain;
        }

        [[nodiscard]] const VkSurfaceKHR &getSurface() const noexcept
        {
            return surface;
        }

        [[nodiscard]] const std::vector<VkImage> &getSwapchainImages() const noexcept
        {
            return swapchainImages;
        }

        [[nodiscard]] const std::vector<VkImageView> &getSwapchainImageViews() const noexcept
        {
            return swapchainImageViews;
        }

        ~VulkanBridge()
        {
            for (auto imageView : swapchainImageViews)
            {
                vkDestroyImageView(vulkanDevice, imageView, nullptr);
            }

            if (swapchain != VK_NULL_HANDLE)
            {
                vkDestroySwapchainKHR(vulkanDevice, swapchain, nullptr);
                swapchain = VK_NULL_HANDLE;
            }

            if (surface != VK_NULL_HANDLE)
            {
                vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
                surface = VK_NULL_HANDLE;
            }
        }

        VulkanBridge(const VulkanBridge &) = delete;
        VulkanBridge &operator=(const VulkanBridge &) = delete;

        VulkanBridge(VulkanBridge &&other) noexcept : vulkanInstance(other.vulkanInstance), vulkanDevice(other.vulkanDevice)
        {
            other.surface = VK_NULL_HANDLE;
            other.swapchain = VK_NULL_HANDLE;
        }

        VulkanBridge &operator=(VulkanBridge &&other) noexcept
        {
            if (this != &other)
            {
                if (surface != VK_NULL_HANDLE)
                {
                    for (auto imageView : swapchainImageViews)
                    {
                        vkDestroyImageView(vulkanDevice, imageView, nullptr);
                    }

                    vkDestroySwapchainKHR(vulkanDevice, swapchain, nullptr);
                    vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
                }
                swapchainImageViews = other.swapchainImageViews;
                surface = other.surface;
                swapchain = other.swapchain;
                other.surface = VK_NULL_HANDLE;
                other.swapchain = VK_NULL_HANDLE;
            }
            return *this;
        }
    };

} //namespace sas