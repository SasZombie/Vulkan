#include "vkMasterPipeline.hpp"

sas::VulkanMasterPipeline::VulkanMasterPipeline(VulkanDevice &vulkanDevice, const VulkanPipelineComponents &components) noexcept
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

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateInfo.pDynamicStates = dynamicStates.data();

    // Master Pipeline Create Info
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderingInfo; 

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
    pipelineInfo.pDynamicState = &dynamicStateInfo;

    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
}