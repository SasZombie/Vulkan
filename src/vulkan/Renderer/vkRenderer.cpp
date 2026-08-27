#include "vkRenderer.hpp"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "Mesh.hpp"

sas::VulkanRenderer::VulkanRenderer(Window &nwindow, Camera &ncamera, VulkanDevices &nvulkanLowLvl, VulkanSharedObjects &sharedObj)
    : window(nwindow),
      camera(ncamera),
      vulkanLowLvl(nvulkanLowLvl),
      sharedObjects(sharedObj),
      viewPort(window),
      components{&vulkanLowLvl.vkBridge, &inputPipeline, &viewPort, &raster, &pixelStyle, &sharedObjects},
      masterPipeline(vulkanLowLvl.vkDevice, components)
{
    pipelinePasses.emplace_back(std::make_unique<MainScenePass>("Main Scene Pass"));
}


//MARK: Draw Frame
void sas::VulkanRenderer::drawFrame(const std::vector<DrawingComponents> &objectsToRender) noexcept
{
    preFrame();

    uint32_t imageIndex = 0;
    getNextImage(imageIndex);

    recordCommandBuffer();

    VkImageMemoryBarrier2 barrierToRender;
    imageLayoutTransitionColor(imageIndex, barrierToRender);

    dynamicRendering(imageIndex);

    setUpRaster({});
    setUpViewPort({});

    setUpShader({});

    RenderPassComponents renderComponents{vulkanLowLvl, masterPipeline, camera};

    for(const auto& pass : pipelinePasses)
    {
        if(pass->isActive)
        {
            pass->record(objectsToRender, renderComponents);
        }
    }

    renderEditorUi();

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
    // TODO: Render Area should maybe be dynamic
    renderingInfo.renderArea = {{0, 0}, {static_cast<unsigned int>(window.getWidth()), static_cast<unsigned int>(window.getHeight())}};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(vulkanLowLvl.vkCommand.getCommandBuffer(), &renderingInfo);
}

void sas::VulkanRenderer::drawCallRecorder(const DrawingComponents &component) const noexcept
{
    const auto &renderObj = *component.get<RenderObject>();

    const auto &comandBuff = vulkanLowLvl.vkCommand.getCommandBuffer();
    const auto &descriptor = renderObj.material->descriptorSet;

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(comandBuff, 0, 1, &renderObj.mesh->vertexBuffer, offsets);
    vkCmdBindIndexBuffer(comandBuff, renderObj.mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        comandBuff,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        masterPipeline.getGraphicsPipelineLayout(),
        0,
        1,
        &descriptor,
        0, nullptr);

    const auto &transform = *component.get<ObjectTransform3D>();
    const auto &modelMat = transform.getModelMatrix();

    const auto &finalMvp = camera.getViewProjection() * modelMat;

    PushConstants constants{finalMvp};

    vkCmdPushConstants(comandBuff,
                       masterPipeline.getGraphicsPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(PushConstants),
                       &constants);

    vkCmdDrawIndexed(comandBuff, renderObj.mesh->indexCount, 1, 0, 0, 0);
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

void sas::VulkanRenderer::setUpShader(const RenderObject &renderObj) const noexcept
{
    const auto &comandBuff = vulkanLowLvl.vkCommand.getCommandBuffer();

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

    VkVertexInputAttributeDescription2EXT attrDesc[4]{};
    attrDesc[0].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attrDesc[0].location = 0; // inPosition
    attrDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDesc[0].offset = offsetof(Vertex, pos);

    attrDesc[1].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attrDesc[1].location = 1; // inNormal
    attrDesc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDesc[1].offset = offsetof(Vertex, normals);

    attrDesc[2].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attrDesc[2].location = 2; // inTextCoords
    attrDesc[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDesc[2].offset = offsetof(Vertex, texCoord);

    attrDesc[3].sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
    attrDesc[3].location = 3; // inColor
    attrDesc[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDesc[3].offset = offsetof(Vertex, color);

    vkCmdSetVertexInputEXT(comandBuff, 1, &bindingDesc, 4, attrDesc);
}

// TODO: This should be per material
// But I havent introduced that notion yet
void sas::VulkanRenderer::setUpRaster(const RenderObject &renderObj) const noexcept
{
    const auto &rasterInfo = raster.getRasterizer();
    const auto &comandBuff = vulkanLowLvl.vkCommand.getCommandBuffer();

    vkCmdSetPolygonModeEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), rasterInfo.polygonMode);
    vkCmdSetCullModeEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), rasterInfo.cullMode);
    vkCmdSetFrontFaceEXT(vulkanLowLvl.vkCommand.getCommandBuffer(), rasterInfo.frontFace);

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
}

void sas::VulkanRenderer::setUpViewPort(const RenderObject &renderObj) const noexcept
{
    const auto &comandBuff = vulkanLowLvl.vkCommand.getCommandBuffer();

    vkCmdSetViewportWithCount(comandBuff, 1, &viewPort.getViewport());
    vkCmdSetScissorWithCount(comandBuff, 1, &viewPort.getScissors());
}

void sas::VulkanRenderer::renderEditorUi() const noexcept
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2. Build your Editor UI
    ImGui::Begin("Inspector");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    // 3. Render ImGui draw lists into Vulkan command buffer
    ImGui::Render();
    ImDrawData *drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, vulkanLowLvl.vkCommand.getCommandBuffer());
}