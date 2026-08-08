#pragma once

#include "volk.h"

#include "vkDevice.hpp"
#include <vector>

namespace sas
{

    struct VulkanShader
    {
        VkShaderEXT shaderModule;
        VkShaderCreateInfoEXT shaderStageInfo{};
    };

    class VulkanShaderPipeline
    {
    private:
        VulkanDevice &device;
        VulkanShader vertShader;
        VulkanShader fragShader;

    public:
        VulkanShaderPipeline(VulkanDevice &vulkanDevice);

        [[nodiscard]] std::pair<VulkanShader, VulkanShader> getShaderStages() const noexcept
        {
            return std::make_pair(vertShader, fragShader);
        }
        VulkanShaderPipeline(const VulkanShaderPipeline &) = delete;
        VulkanShaderPipeline &operator=(const VulkanShaderPipeline &) = delete;

        VulkanShaderPipeline(VulkanShaderPipeline &&other) noexcept
            : device(other.device), vertShader(other.vertShader), fragShader(other.fragShader)
        {
            other.fragShader.shaderModule = nullptr;
            other.vertShader.shaderModule = nullptr;
        }

        VulkanShaderPipeline &operator=(VulkanShaderPipeline &&other) noexcept
        {
            if (this != &other)
            {
                if (vertShader.shaderModule)
                {
                    vkDestroyShaderEXT(device, vertShader.shaderModule, nullptr);
                }

                if (fragShader.shaderModule)
                {
                    vkDestroyShaderEXT(device, fragShader.shaderModule, nullptr);
                }

                vertShader.shaderModule = other.vertShader.shaderModule;
                fragShader.shaderModule = other.fragShader.shaderModule;
                other.vertShader.shaderModule = nullptr;
                other.fragShader.shaderModule = nullptr;
            }
            return *this;
        }

        ~VulkanShaderPipeline()
        {
            if (vertShader.shaderModule)
            {
                vkDestroyShaderEXT(device, vertShader.shaderModule, nullptr);
            }

            if (fragShader.shaderModule)
            {
                vkDestroyShaderEXT(device, fragShader.shaderModule, nullptr);
            }
        }
    };

} // namespace sas