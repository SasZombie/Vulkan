#pragma once

#include "vulkan/vulkan.h"

class VulkanRastarization
{
private:
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multisampling{};

public:
    VulkanRastarization() noexcept;

    const VkPipelineRasterizationStateCreateInfo& getRasterizer() const noexcept
    {
        return rasterizer;
    }

    const VkPipelineMultisampleStateCreateInfo& getMultisampling() const noexcept
    {
        return multisampling;
    }
};

