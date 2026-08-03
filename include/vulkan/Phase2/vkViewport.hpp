#pragma once

#include "vulkan/vulkan.h"

class Window;

class VulkanViewport
{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportState;

public: 
    VulkanViewport(const Window& window);

    VkViewport getViewport() const noexcept
    {
        return viewport;
    }

    VkRect2D getScissors() const noexcept
    {
        return scissor;
    }

    const VkPipelineViewportStateCreateInfo& getViewportState() const noexcept
    {
        return viewportState;
    }
};