#pragma once

#include "vkBridge.hpp"
#include "vkShaderPipeline.hpp"
#include "vkInputPipeline.hpp"
#include "vkViewport.hpp"
#include "vkRastarization.hpp"
#include "vkPixelStyling.hpp"

namespace sas
{

    struct VulkanPipelineComponents
    {
        VulkanBridge *bridge;
        VulkanShaderPipeline *shaderPipeline;
        VulkanInputPipeline *inputPipeline;
        VulkanViewport *viewport;
        VulkanRastarization *rastar;
        VulkanPixelStyling *pixelStyling;
    };

} // namespace sas
