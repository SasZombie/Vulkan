#pragma once

#include "vkInitialize.hpp"
#include "vkPhysicalDevices.hpp"
#include "vkDevice.hpp"
#include "window.hpp"
#include "vkBridge.hpp"
#include "vkCommand.hpp"

// Phase 2
#include "vkShaderPipeline.hpp"
#include "vkInputPipeline.hpp"
#include "vkViewport.hpp"
#include "vkPixelStyling.hpp"
#include "vkRastarization.hpp"
// Final Pipe

#include "vkMasterPipeline.hpp"

class VulkanRenderer
{
public:
    VulkanRenderer(Window &window);

    VulkanRenderer(VulkanRenderer &&) noexcept = default;
    VulkanRenderer &operator=(VulkanRenderer &&) noexcept = default;

    VulkanRenderer(const VulkanRenderer &) = delete;
    VulkanRenderer &operator=(const VulkanRenderer &) = delete;

    ~VulkanRenderer() noexcept
    {
        if(vkDevice.getDevice() != nullptr)
        {
            vkDeviceWaitIdle(vkDevice);
        }
    }

    void drawFrame() noexcept;

private:
    Window& window;
    VulkanInstanceWrapper vk;
    VulkanPhysicalDevice vkPhysical;
    VulkanDevice vkDevice;
    VulkanBridge vkBridge;
    VulkanCommand vkCommand;

    VulkanInputPipeline inputPipeline;
    VulkanShaderPipeline shaderPipeline;
    VulkanViewport viewPort;
    VulkanPixelStyling pixelStyle;
    VulkanRastarization raster;

    VulkanPipelineComponents components;
    VulkanMasterPipeline masterPipeline;

    void preFrame() const noexcept;
    void getNextImage(uint32_t& imageIndex) const noexcept;

    void recordCommandBuffer() const noexcept;
    void imageLayoutTransitionColor(uint32_t imageIndex, VkImageMemoryBarrier2& barrierToRender) const noexcept;

    void dynamicRendering(uint32_t imageIndex) const noexcept;

    void drawCall() const noexcept;

    void imageLayoutTransitionPresent(VkImageMemoryBarrier2& barrierToRender) const noexcept;

    void gpuCall() const noexcept;

    void presentImageToWindow(uint32_t imageIndex) const noexcept;
};