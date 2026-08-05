#include "vkRenderer.hpp"

sas::VulkanRenderer::VulkanRenderer(Window &nwindow, Camera &ncamera)
    : window(nwindow),
      camera(ncamera),
      vk(),
      vkPhysical(vk),
      vkDevice(vkPhysical),
      vkBridge(window, vk, vkPhysical, vkDevice),
      vkCommand(vkDevice),
      shaderPipeline(vkDevice),
      viewPort(window),
      components{&vkBridge, &shaderPipeline, &inputPipeline, &viewPort, &raster, &pixelStyle},
      masterPipeline(vkDevice, components)
{
}

void sas::VulkanRenderer::drawFrame() noexcept
{
    preFrame();

    uint32_t imageIndex = 0;
    getNextImage(imageIndex);

    recordCommandBuffer();

    VkImageMemoryBarrier2 barrierToRender;
    imageLayoutTransitionColor(imageIndex, barrierToRender);

    dynamicRendering(imageIndex);

    drawCall();
    imageLayoutTransitionPresent(barrierToRender);

    gpuCall(imageIndex);

    presentImageToWindow(imageIndex);

    vkCommand.advanceFrame();
}

void sas::VulkanRenderer::preFrame() const noexcept
{
    vkWaitForFences(vkDevice, 1, &vkCommand.getFence(), VK_TRUE, UINT64_MAX);
}

void sas::VulkanRenderer::getNextImage(uint32_t &imageIndex) const noexcept
{
    vkAcquireNextImageKHR(vkDevice, vkBridge.getSwapChain(), UINT64_MAX, vkCommand.getImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);
    vkResetFences(vkDevice, 1, &vkCommand.getFence());
}

void sas::VulkanRenderer::recordCommandBuffer() const noexcept
{
    vkResetCommandBuffer(vkCommand.getCommandBuffer(), 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(vkCommand.getCommandBuffer(), &beginInfo);
}

void sas::VulkanRenderer::imageLayoutTransitionColor(uint32_t imageIndex, VkImageMemoryBarrier2 &barrierToRender) const noexcept
{
    barrierToRender.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrierToRender.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    barrierToRender.srcAccessMask = 0;
    barrierToRender.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    barrierToRender.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    barrierToRender.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrierToRender.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrierToRender.image = vkBridge.getSwapchainImages()[imageIndex];
    barrierToRender.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierToRender.subresourceRange.levelCount = 1;
    barrierToRender.subresourceRange.layerCount = 1;

    VkDependencyInfo depInfoToRender{};
    depInfoToRender.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfoToRender.imageMemoryBarrierCount = 1;
    depInfoToRender.pImageMemoryBarriers = &barrierToRender;
    vkCmdPipelineBarrier2(vkCommand.getCommandBuffer(), &depInfoToRender);
}

void sas::VulkanRenderer::dynamicRendering(uint32_t imageIndex) const noexcept
{
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = vkBridge.getSwapchainImageViews()[imageIndex];
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

    vkCmdBeginRendering(vkCommand.getCommandBuffer(), &renderingInfo);
}

void sas::VulkanRenderer::drawCall() const noexcept
{
    vkCmdBindPipeline(vkCommand.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, masterPipeline.getGraphicsPipeline());

    vkCmdSetViewport(vkCommand.getCommandBuffer(), 0, 1, &viewPort.getViewport());
    vkCmdSetScissor(vkCommand.getCommandBuffer(), 0, 1, &viewPort.getScissors());

    PushConstants constants{camera.getMVP()};
    // Draw 3 vertices (1 instance). No vertex buffers needed because of our hardcoded gl_VertexIndex shader!

    sas::math::Mat4 proj = math::perspective(math::degToRad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    vkCmdPushConstants(
        vkCommand.getCommandBuffer(),
        masterPipeline.getGraphicsPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(PushConstants),
        &constants.mvp);

    vkCmdDraw(vkCommand.getCommandBuffer(), 3, 1, 0, 0);

    vkCmdEndRendering(vkCommand.getCommandBuffer());
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
    vkCmdPipelineBarrier2(vkCommand.getCommandBuffer(), &depInfoToPresent);

    vkEndCommandBuffer(vkCommand.getCommandBuffer());
}

void sas::VulkanRenderer::gpuCall(uint32_t imageIndex) const noexcept
{
    VkSemaphoreSubmitInfo waitSemaphoreInfo{};
    waitSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitSemaphoreInfo.semaphore = vkCommand.getImageAvailableSemaphore();
    waitSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSemaphoreSubmitInfo signalSemaphoreInfo{};
    signalSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signalSemaphoreInfo.semaphore = vkCommand.getRendererFinishedSemaphore();
    signalSemaphoreInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    VkCommandBufferSubmitInfo cmdSubmitInfo{};
    cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmdSubmitInfo.commandBuffer = vkCommand.getCommandBuffer();

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submitInfo.waitSemaphoreInfoCount = 1;
    submitInfo.pWaitSemaphoreInfos = &waitSemaphoreInfo; // Wait for image acquisition
    submitInfo.signalSemaphoreInfoCount = 1;
    submitInfo.pSignalSemaphoreInfos = &signalSemaphoreInfo; // Signal when render finishes
    submitInfo.commandBufferInfoCount = 1;
    submitInfo.pCommandBufferInfos = &cmdSubmitInfo;

    // Submit work to GPU and signal inFlightFence when complete
    vkQueueSubmit2(vkDevice.getQueue(), 1, &submitInfo, vkCommand.getFence());
}

void sas::VulkanRenderer::presentImageToWindow(uint32_t imageIndex) const noexcept
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &vkCommand.getRendererFinishedSemaphore();
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkBridge.getSwapChain();
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(vkDevice.getQueue(), &presentInfo);
}
