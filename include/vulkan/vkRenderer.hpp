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
#include "RenderObject.hpp"

#include "EntityRegistry.hpp"
#include "ObjectTransform.hpp"


namespace sas
{

    //TODO: Renderer should have multiple sub-renderers to take advantage
    //Of how vulkan works
    using DrawingComponents = sas::Combined<sas::RenderObject, sas::ObjectTransform3D>;

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

        void drawFrame(const std::vector<DrawingComponents>& objectsToRender) noexcept;

    public:
        Window &window;
        Camera &camera;
        VulkanDevices& vulkanLowLvl;
        VulkanSharedObjects& sharedObjects;

        VulkanInputPipeline inputPipeline;
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

        void drawCallRecorder(const DrawingComponents& component) const noexcept;

        void imageLayoutTransitionPresent(VkImageMemoryBarrier2 &barrierToRender) const noexcept;

        void gpuCall(uint32_t imageIndex) const noexcept;

        void presentImageToWindow(uint32_t imageIndex) const noexcept;

        void setUpShader(const RenderObject &renderObj) const noexcept;
        void setUpRaster(const RenderObject &renderObj) const noexcept;
        void setUpViewPort(const RenderObject &renderObj) const noexcept;
    };

} // namespace sas
