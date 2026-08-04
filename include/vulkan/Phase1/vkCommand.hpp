#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "vkDevice.hpp"

constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

class VulkanCommand
{
private:
    VulkanDevice &device;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    
    std::vector<VkCommandBuffer> commandBuff;
    std::vector<VkSemaphore> imageAvailableSemaphore;
    std::vector<VkSemaphore> renderFinishedSemaphore;
    std::vector<VkFence> inFlightFence;

public:
    VulkanCommand(VulkanDevice &ndevice);

    [[nodiscard]]const VkFence& getFence() const noexcept
    {
        return inFlightFence;
    }

    [[nodiscard]]const VkSemaphore& getImageAvailableSemaphore() const noexcept
    {
        return imageAvailableSemaphore;
    }

    [[nodiscard]]const VkSemaphore& getRendererFinishedSemaphore() const noexcept
    {
        return renderFinishedSemaphore;
    }

    [[nodiscard]]const VkCommandBuffer& getCommandBuffer() const noexcept
    {
        return commandBuff;
    }
    
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
