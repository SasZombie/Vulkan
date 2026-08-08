#pragma once

#include "volk.h"

namespace sas
{

    class Window;

    class VulkanViewport
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportState;

    public:
        VulkanViewport(const Window &window);

        [[nodiscard]]const VkViewport &getViewport() const noexcept
        {
            return viewport;
        }

        [[nodiscard]]const VkRect2D &getScissors() const noexcept
        {
            return scissor;
        }

        [[nodiscard]]const VkPipelineViewportStateCreateInfo &getViewportState() const noexcept
        {
            return viewportState;
        }
    };

} // namspace sas
