#pragma once

#include "volk.h"

namespace sas
{

    class VulkanPixelStyling
    {
    private:
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        VkPipelineDepthStencilStateCreateInfo depthStencil{};

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    public:
        VulkanPixelStyling() noexcept;

        [[nodiscard]] const VkPipelineColorBlendAttachmentState &getColorBlendAttachment() const noexcept
        {
            return colorBlendAttachment;
        }

        [[nodiscard]] const VkPipelineDepthStencilStateCreateInfo &getDepthStencil() const noexcept
        {
            return depthStencil;
        }

        [[nodiscard]] const VkPipelineColorBlendStateCreateInfo &getColorBlending() const noexcept
        {
            return colorBlending;
        }
    };
} // namespace sas
