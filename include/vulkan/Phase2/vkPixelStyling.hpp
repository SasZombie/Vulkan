#pragma once

#include "vulkan/vulkan.h"

class VulkanPixelStyling
{
private:

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    VkPipelineDepthStencilStateCreateInfo depthStencil{};

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    
public:
    VulkanPixelStyling() noexcept;


    const VkPipelineColorBlendAttachmentState& getColorBlendAttachment() const noexcept
    {
        return colorBlendAttachment;
    }

    const VkPipelineDepthStencilStateCreateInfo& getDepthStencil() const noexcept
    {
        return depthStencil;
    }

    const VkPipelineColorBlendStateCreateInfo& getColorBlending() const noexcept
    {
        return colorBlending;
    }

};