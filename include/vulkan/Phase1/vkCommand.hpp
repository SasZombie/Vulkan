#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "vkDevice.hpp"

constexpr size_t MAX_FRAMES_IN_FLIGHT = 3;

class VulkanCommand
{
private:
    VulkanDevice &device;
    VkCommandPool commandPool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> commandBuff;
    std::vector<VkSemaphore> imageAvailableSemaphore;
    std::vector<VkSemaphore> renderFinishedSemaphore;
    std::vector<VkFence> inFlightFence;

    uint32_t currentFrame = 0;

public:
    VulkanCommand(VulkanDevice &ndevice);

    [[nodiscard]] const VkFence &getFence() const noexcept
    {
        return inFlightFence[currentFrame];
    }

    [[nodiscard]] const VkSemaphore &getImageAvailableSemaphore() const noexcept
    {
        return imageAvailableSemaphore[currentFrame];
    }

    [[nodiscard]] const VkSemaphore &getRendererFinishedSemaphore() const noexcept
    {
        return renderFinishedSemaphore[currentFrame];
    }

    [[nodiscard]] const VkCommandBuffer &getCommandBuffer() const noexcept
    {
        return commandBuff[currentFrame];
    }

    [[nodiscard]] uint32_t getCurrentFrame() const noexcept
    {
        return currentFrame;
    }

    void advanceFrame() noexcept
    {
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    ~VulkanCommand() noexcept
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (inFlightFence[i])
            {
                vkDestroyFence(device, inFlightFence[i], nullptr);
            }

            if (renderFinishedSemaphore[i])
            {
                vkDestroySemaphore(device, renderFinishedSemaphore[i], nullptr);
            }

            if (imageAvailableSemaphore[i])
            {
                vkDestroySemaphore(device, imageAvailableSemaphore[i], nullptr);
            }
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
        other.commandBuff.clear();
        other.commandPool = nullptr;
        other.imageAvailableSemaphore.clear();
        other.inFlightFence.clear();
        other.renderFinishedSemaphore.clear();
    }

    VulkanCommand &operator=(VulkanCommand &&other) noexcept
    {
        if (this != &other)
        {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            {

                if (inFlightFence[i])
                {
                    vkDestroyFence(device, inFlightFence[i], nullptr);
                }

                if (renderFinishedSemaphore[i])
                {
                    vkDestroySemaphore(device, renderFinishedSemaphore[i], nullptr);
                }

                if (imageAvailableSemaphore[i])
                {
                    vkDestroySemaphore(device, imageAvailableSemaphore[i], nullptr);
                }
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
