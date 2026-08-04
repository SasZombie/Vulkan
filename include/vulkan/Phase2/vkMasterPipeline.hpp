#pragma once

#include "vulkan/vulkan.h"
#include "vkDevice.hpp"

#include "vkComponents.hpp"


class VulkanMasterPipeline
{
private:
    VulkanDevice &device;
    VkPipelineLayout pipelineLayout = nullptr;
    VkPipeline graphicsPipeline = nullptr;

    void cleanUp() noexcept
    {
        if(graphicsPipeline)
        {
            vkDestroyPipeline(device, graphicsPipeline, nullptr);
        }

        if(pipelineLayout)
        {

            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        }
    }

public:
    VulkanMasterPipeline(VulkanDevice &vulkanDevice, const VulkanPipelineComponents& components) noexcept;

    [[nodiscard]] const VkPipeline& getGraphicsPipeline() const noexcept
    {
        return graphicsPipeline;
    }

    ~VulkanMasterPipeline() noexcept
    {
        cleanUp();
    }
};