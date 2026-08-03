#pragma once

#include <vulkan/vulkan.h>
#include "vkDevice.hpp"

class VulkanCommand
{
private:
    VulkanDevice &device;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuff = VK_NULL_HANDLE;

    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence inFlightFence = VK_NULL_HANDLE;

public:
    VulkanCommand(VulkanDevice &ndevice);
    ~VulkanCommand() noexcept
    {
        if (inFlightFence)
        {
            vkDestroyFence(device, inFlightFence, nullptr);
        }

        if (renderFinishedSemaphore)
        {
            vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
        }

        if (imageAvailableSemaphore)
        {
            vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
        }

        if (commandPool)
        {
            vkDestroyCommandPool(device, commandPool, nullptr);
        }
    }

    VulkanCommand(const VulkanCommand &) = delete;
    VulkanCommand &operator=(const VulkanCommand &) = delete;

    VulkanCommand(VulkanCommand &&other) noexcept
        : device(other.device), commandPool(other.commandPool), commandBuff(other.commandBuff),
          imageAvailableSemaphore(other.imageAvailableSemaphore), renderFinishedSemaphore(other.renderFinishedSemaphore),
          inFlightFence(other.inFlightFence)
    {
        other.commandBuff = nullptr;
        other.commandPool = nullptr;
        other.imageAvailableSemaphore = nullptr;
        other.inFlightFence = nullptr;
        other.renderFinishedSemaphore = nullptr;
    }

    VulkanCommand &operator=(VulkanCommand &&other) noexcept
    {
        if (this != &other)
        {
            if (inFlightFence)
            {
                vkDestroyFence(device, inFlightFence, nullptr);
            }

            if (renderFinishedSemaphore)
            {
                vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
            }

            if (imageAvailableSemaphore)
            {
                vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
            }

            if (commandPool)
            {
                vkDestroyCommandPool(device, commandPool, nullptr);
            }

            inFlightFence = other.inFlightFence;
            imageAvailableSemaphore = other.imageAvailableSemaphore;
            commandPool = other.commandPool;
            renderFinishedSemaphore = other.renderFinishedSemaphore;
            commandBuff = other.commandBuff;
        }
        return *this;
    }
};
