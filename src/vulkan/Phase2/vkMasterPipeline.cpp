#include "vkMasterPipeline.hpp"

sas::VulkanMasterPipeline::VulkanMasterPipeline(VulkanDevice &vulkanDevice, const VulkanPipelineComponents &components) noexcept
    : device(vulkanDevice)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstants);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    pipelineLayoutInfo.setLayoutCount = 1;

    const auto &layout = components.sharedObjects->vulkanDescriptor.getDescriptorLayout();

    pipelineLayoutInfo.pSetLayouts = &layout;

    vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
}