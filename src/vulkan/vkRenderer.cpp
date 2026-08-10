#include "vkRenderer.hpp"
#include "Mesh.hpp"
sas::VulkanRenderer::VulkanRenderer(Window &nwindow, Camera &ncamera, VulkanDevices &nvulkanLowLvl)
    : window(nwindow),
      camera(ncamera),
      vulkanLowLvl(nvulkanLowLvl),
      dynamicShaderPipeline(vulkanLowLvl.vkDevice),
      shaderPipeline(vulkanLowLvl.vkDevice),
      viewPort(window),
      components{&vulkanLowLvl.vkBridge, &shaderPipeline, &inputPipeline, &viewPort, &raster, &pixelStyle},
      masterPipeline(vulkanLowLvl.vkDevice, components)
{
}

void sas::VulkanRenderer::drawFrame(const std::vector<RenderObject> &objectsToRender) noexcept
{
    preFrame();

    uint32_t imageIndex = 0;
    getNextImage(imageIndex);

    recordCommandBuffer();

    VkImageMemoryBarrier2 barrierToRender;
    imageLayoutTransitionColor(imageIndex, barrierToRender);

    dynamicRendering(imageIndex);

    for (const auto &renderObj : objectsToRender)
    {
        // drawCallRecorderDynamic(renderObj);
        drawCallRecorder(renderObj);
    }

    vkCmdEndRendering(vulkanLowLvl.vkCommand.getCommandBuffer());

    imageLayoutTransitionPresent(barrierToRender);

    gpuCall(imageIndex);

    presentImageToWindow(imageIndex);

    vulkanLowLvl.vkCommand.advanceFrame();
}

void sas::VulkanRenderer::preFrame() const noexcept
{
    vkWaitForFences(vulkanLowLvl.vkDevice, 1, &vulkanLowLvl.vkCommand.getFence(), VK_TRUE, UINT64_MAX);
}

void sas::VulkanRenderer::getNextImage(uint32_t &imageIndex) const noexcept
{
    vkAcquireNextImageKHR(vulkanLowLvl.vkDevice, vulkanLowLvl.vkBridge.getSwapChain(), UINT64_MAX, vulkanLowLvl.vkCommand.getImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);
    vkResetFences(vulkanLowLvl.vkDevice, 1, &vulkanLowLvl.vkCommand.getFence());
}

void sas::VulkanRenderer::recordCommandBuffer() const noexcept
{
    vkResetCommandBuffer(vulkanLowLvl.vkCommand.getCommandBuffer(), 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(vulkanLowLvl.vkCommand.getCommandBuffer(), &beginInfo);
}

void sas::VulkanRenderer::imageLayoutTransitionColor(uint32_t imageIndex, VkImageMemoryBarrier2 &barrierToRender) const noexcept
{
    barrierToRender.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierToRender.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierToRender.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrierToRender.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    barrierToRender.srcAccessMask = 0;
    barrierToRender.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    barrierToRender.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    barrierToRender.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierToRender.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrierToRender.image = vulkanLowLvl.vkBridge.getSwapchainImages()[imageIndex];
    barrierToRender.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierToRender.subresourceRange.levelCount = 1;
    barrierToRender.subresourceRange.layerCount = 1;

    barrierToRender.subresourceRange.baseArrayLayer = 0;
    barrierToRender.subresourceRange.baseArrayLayer = 0;

    VkDependencyInfo depInfoToRender{};
    depInfoToRender.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfoToRender.imageMemoryBarrierCount = 1;
    depInfoToRender.pImageMemoryBarriers = &barrierToRender;
    vkCmdPipelineBarrier2(vulkanLowLvl.vkCommand.getCommandBuffer(), &depInfoToRender);
}

void sas::VulkanRenderer::dynamicRendering(uint32_t imageIndex) const noexcept
{
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = vulkanLowLvl.vkBridge.getSwapchainImageViews()[imageIndex];
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // BackGround
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = {{0, 0}, {static_cast<unsigned int>(window.getWidth()), static_cast<unsigned int>(window.getHeight())}};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(vulkanLowLvl.vkCommand.getCommandBuffer(), &renderingInfo);
}

void sas::VulkanRenderer::drawCallRecorderDynamic(const RenderObject &renderObj) const noexcept
{
    const auto &rasterInfo = raster.getRasterizer();
    const auto &comandBuff = vulkanLowLvl.vkCommand.getCommandBuffer();

    vkCmdSetRasterizationSamplesEXT(comandBuff, raster.getMultisampling().rasterizationSamples);

    vkCmdSetAlphaToCoverageEnableEXT(comandBuff, VK_FALSE);

    vkCmdSetPrimitiveRestartEnable(comandBuff, VK_FALSE);
    vkCmdSetStencilTestEnable(comandBuff, VK_FALSE);
    vkCmdSetDepthTestEnable(comandBuff, VK_TRUE);
    vkCmdSetDepthWriteEnable(comandBuff, VK_TRUE);
    vkCmdSetRasterizerDiscardEnable(comandBuff, VK_FALSE);
    vkCmdSetDepthCompareOp(comandBuff, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkSampleMask sampleMask = 0xFFFFFFFF;
    vkCmdSetSampleMaskEXT(comandBuff, raster.getMultisampling().rasterizationSamples, &sampleMask);


    vkCmdSetDepthBiasEnable(comandBuff, VK_FALSE);

    vkCmdSetPrimitiveTopologyEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), inputPipeline.getInputAssembly().topology);
    vkCmdSetPolygonModeEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), rasterInfo.polygonMode);
    vkCmdSetCullModeEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), rasterInfo.cullMode);
    vkCmdSetFrontFaceEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), rasterInfo.frontFace);

    vkCmdSetViewportWithCount(comandBuff, 1, &viewPort.getViewport());
    vkCmdSetScissorWithCount(comandBuff, 1, &viewPort.getScissors());

    VkBool32 blendEnable = VK_FALSE;
    vkCmdSetColorBlendEnableEXT(comandBuff, 0, 1, &blendEnable);
    VkColorComponentFlags colorMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    vkCmdSetColorWriteMaskEXT(comandBuff, 0, 1, &colorMask);

    VkVertexInputBindingDescription2EXT bindingDesc{};
    bindingDesc.sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(Vertex);
    bindingDesc.divisor = 1;
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription2EXT attrDesc[2]{};
    attrDesc[0].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attrDesc[0].location = 0; // inPosition
    attrDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDesc[0].offset = offsetof(Vertex, pos);

    attrDesc[1].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attrDesc[1].location = 1; // inColor
    attrDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDesc[1].offset = offsetof(Vertex, color);

    vkCmdSetVertexInputEXT(comandBuff, 1, &bindingDesc, 2, attrDesc);

    VkShaderStageFlagBits stages[] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
    const auto &[vert, frag] = renderObj.shader->getShaderStages();

    VkShaderEXT shaders[] = {vert.shaderModule, frag.shaderModule};

    vkCmdBindShadersEXT(comandBuff, 2, stages, shaders);

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(comandBuff, 0, 1, &renderObj.vertexBuffer, offsets);
    vkCmdBindIndexBuffer(comandBuff, renderObj.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    PushConstants constants{camera.getMVP()};

    vkCmdPushConstants(comandBuff,
                       masterPipeline.getGraphicsPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(PushConstants),
                       &constants);

    vkCmdDrawIndexed(comandBuff, renderObj.indexCount, 1, 0, 0, 0);
}

void sas::VulkanRenderer::drawCallRecorder(const RenderObject& renderObj) const noexcept
{
    vkCmdBindPipeline(vulkanLowLvl.vkCommand.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, masterPipeline.getGraphicsPipeline());

    vkCmdSetViewport(vulkanLowLvl.vkCommand.getCommandBuffer(), 0, 1, &viewPort.getViewport());
    vkCmdSetScissor(vulkanLowLvl.vkCommand.getCommandBuffer(), 0, 1, &viewPort.getScissors());

    VkBuffer buffers[] = {renderObj.vertexBuffer};

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(vulkanLowLvl.vkCommand.getCommandBuffer(), 0, 1, buffers, offsets);
    vkCmdBindIndexBuffer(vulkanLowLvl.vkCommand.getCommandBuffer(), renderObj.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    PushConstants constants{camera.getMVP()};

    vkCmdPushConstants(
        vulkanLowLvl.vkCommand.getCommandBuffer(),
        masterPipeline.getGraphicsPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(PushConstants),
        &constants.mvp);

    vkCmdDrawIndexed(vulkanLowLvl.vkCommand.getCommandBuffer(), static_cast<uint32_t>(renderObj.indexCount), 1, 0, 0, 0);
}

void sas::VulkanRenderer::imageLayoutTransitionPresent(VkImageMemoryBarrier2 &barrierToRender) const noexcept
{
    VkImageMemoryBarrier2 barrierToPresent = barrierToRender;
    barrierToPresent.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrierToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierToPresent.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    barrierToPresent.dstAccessMask = 0;

    VkDependencyInfo depInfoToPresent{};
    depInfoToPresent.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfoToPresent.imageMemoryBarrierCount = 1;
    depInfoToPresent.pImageMemoryBarriers = &barrierToPresent;
    vkCmdPipelineBarrier2(vulkanLowLvl.vkCommand.getCommandBuffer(), &depInfoToPresent);

    vkEndCommandBuffer(vulkanLowLvl.vkCommand.getCommandBuffer());
}

void sas::VulkanRenderer::gpuCall(uint32_t imageIndex) const noexcept
{
    VkSemaphoreSubmitInfo waitSemaphoreInfo{};
    waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSemaphoreInfo.semaphore = vulkanLowLvl.vkCommand.getImageAvailableSemaphore();
    waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSemaphoreSubmitInfo signalSemaphoreInfo{};
    signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalSemaphoreInfo.semaphore = vulkanLowLvl.vkCommand.getRendererFinishedSemaphore();
    signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    VkCommandBufferSubmitInfo cmdSubmitInfo{};
    cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmdSubmitInfo.commandBuffer = vulkanLowLvl.vkCommand.getCommandBuffer();

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo; // Wait for image acquisition
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo; // Signal when render finishes
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &cmdSubmitInfo;

    // Submit work to GPU and signal inFlightFence when complete
    vkQueueSubmit2(vulkanLowLvl.vkDevice.getQueue(), 1, &submitInfo, vulkanLowLvl.vkCommand.getFence());
}

void sas::VulkanRenderer::presentImageToWindow(uint32_t imageIndex) const noexcept
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &vulkanLowLvl.vkCommand.getRendererFinishedSemaphore();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vulkanLowLvl.vkBridge.getSwapChain();
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(vulkanLowLvl.vkDevice.getQueue(), &presentInfo);
}
