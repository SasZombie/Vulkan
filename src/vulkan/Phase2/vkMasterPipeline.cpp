#include "vkMasterPipeline.hpp"

VulkanMasterPipeline::VulkanMasterPipeline(VulkanDevice &vulkanDevice, const VulkanPipelineComponents &components) noexcept
    : device(vulkanDevice)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = components.pixelStyling->getColorBlending().attachmentCount;

    VkFormat colorFormat = components.bridge->getSurfaceFormat().format;
    renderingInfo.pColorAttachmentFormats = &colorFormat;

    // Master Pipeline Create Info
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderingInfo; // Dynamic rendering hook!

    pipelineInfo.stageCount = 2;

    const auto &[vertShader, fragShader] = components.shaderPipeline->getShaderStages();

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShader.shaderStageInfo,
        fragShader.shaderStageInfo};

    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &components.inputPipeline->getVertexInput();

    pipelineInfo.pInputAssemblyState = &components.inputPipeline->getInputAssembly();

    pipelineInfo.pViewportState = &components.viewport->getViewportState();
    pipelineInfo.pRasterizationState = &components.rastar->getRasterizer();
    pipelineInfo.pMultisampleState = &components.rastar->getMultisampling(); // Set count to VK_SAMPLE_COUNT_1_BIT
    pipelineInfo.pDepthStencilState = &components.pixelStyling->getDepthStencil();

    pipelineInfo.pColorBlendState = &components.pixelStyling->getColorBlending();
    pipelineInfo.layout = pipelineLayout;

    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
}