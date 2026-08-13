#pragma once

#include "vkInitialize.hpp"
#include "vkPhysicalDevices.hpp"
#include "vkDevice.hpp"
#include "Window.hpp"
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
#include "Camera.hpp"

namespace sas
{

    class VulkanRenderer
    {
    public:
        VulkanRenderer(Window &window, Camera& camera, VulkanDevices& vulkanLowLvl, VulkanSharedObjects& sharedObjs);

        VulkanRenderer(VulkanRenderer &&) noexcept = default;
        VulkanRenderer &operator=(VulkanRenderer &&) noexcept = default;

        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;

        ~VulkanRenderer() noexcept
        {
            if (vulkanLowLvl.vkDevice.getDevice() != nullptr)
            {
                vkDeviceWaitIdle(vulkanLowLvl.vkDevice);
            }
        }

        void drawFrame(const std::vector<RenderObject>& objectsToRender) noexcept;

    public:
        Window &window;
        Camera &camera;
        VulkanDevices& vulkanLowLvl;
        VulkanSharedObjects& sharedObjects;

        VulkanInputPipeline inputPipeline;
        VulkanDynamicShader dynamicShaderPipeline;
        VulkanViewport viewPort;
        VulkanPixelStyling pixelStyle;
        VulkanRastarization raster;

        VulkanPipelineComponents components;
        VulkanMasterPipeline masterPipeline;

        void preFrame() const noexcept;
        void getNextImage(uint32_t &imageIndex) const noexcept;

        void recordCommandBuffer() const noexcept;
        void imageLayoutTransitionColor(uint32_t imageIndex, VkImageMemoryBarrier2 &barrierToRender) const noexcept;

        void dynamicRendering(uint32_t imageIndex) const noexcept;

        void drawCallRecorderDynamic(const RenderObject& renderObj) const noexcept;

        void imageLayoutTransitionPresent(VkImageMemoryBarrier2 &barrierToRender) const noexcept;

        void gpuCall(uint32_t imageIndex) const noexcept;

        void presentImageToWindow(uint32_t imageIndex) const noexcept;

        void setUpShader() const noexcept;
    };

} // namespace sas
