#pragma once

#include "vulkan/vulkan.h"

namespace sas
{

    class VulkanRastarization
    {
    private:
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};

    public:
        VulkanRastarization() noexcept;

        [[nodiscard]] const VkPipelineRasterizationStateCreateInfo &getRasterizer() const noexcept
        {
            return rasterizer;
        }

        [[nodiscard]] const VkPipelineMultisampleStateCreateInfo &getMultisampling() const noexcept
        {
            return multisampling;
        }
    };

} // namespace sas