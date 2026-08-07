#pragma once

#include "vkBridge.hpp"
#include "vkShaderPipeline.hpp"
#include "vkInputPipeline.hpp"
#include "vkViewport.hpp"
#include "vkRastarization.hpp"
#include "vkPixelStyling.hpp"
#include "Math.hpp"
namespace sas
{

    struct PushConstants
    {
        math::Mat4 mvp;
    };

    struct VulkanPipelineComponents
    {
        VulkanBridge *bridge;
        VulkanShaderPipeline *shaderPipeline;
        VulkanInputPipeline *inputPipeline;
        VulkanViewport *viewport;
        VulkanRastarization *rastar;
        VulkanPixelStyling *pixelStyling;
    };

    struct RenderObject
    {
        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
        size_t indexCount;
    };

} // namespace sas
