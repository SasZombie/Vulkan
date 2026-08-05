#pragma once

#include "vulkan/vulkan.h"

namespace sas
{

    class VulkanInputPipeline
    {
    private:
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

    public:
        VulkanInputPipeline() noexcept;

        [[nodiscard]] const VkPipelineInputAssemblyStateCreateInfo &getInputAssembly() const noexcept
        {
            return inputAssembly;
        }

        [[nodiscard]] const VkPipelineVertexInputStateCreateInfo &getVertexInput() const noexcept
        {
            return vertexInputInfo;
        }
    };
} // namespace sas
