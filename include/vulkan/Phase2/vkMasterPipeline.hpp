#pragma once

#include "volk.h"
#include "vkDevice.hpp"

#include "vkComponents.hpp"

namespace sas
{

    class VulkanMasterPipeline
    {
    private:
        VulkanDevice &device;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;

        void cleanUp() noexcept
        {
            if (graphicsPipeline)
            {
                vkDestroyPipeline(device, graphicsPipeline, nullptr);
            }

            if (pipelineLayout)
            {

                vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            }
        }

    public:
        VulkanMasterPipeline(VulkanDevice &vulkanDevice, const VulkanPipelineComponents &components) noexcept;

        [[nodiscard]] const VkPipeline &getGraphicsPipeline() const noexcept
        {
            return graphicsPipeline;
        }

        [[nodiscard]] const VkPipelineLayout &getGraphicsPipelineLayout() const noexcept
        {
            return pipelineLayout;
        }

        ~VulkanMasterPipeline() noexcept
        {
            cleanUp();
        }
    };

} // namespace sas
