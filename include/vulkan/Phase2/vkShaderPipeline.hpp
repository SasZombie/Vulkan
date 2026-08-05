#pragma once

#include "vulkan/vulkan.h"

#include "vkDevice.hpp"
#include <vector>

namespace sas
{

    struct VulkanShader
    {
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
    };

    class VulkanShaderPipeline
    {
    private:
        VulkanDevice &device;
        VulkanShader vertShader;
        VulkanShader fragShader;

        void populateShader(VulkanShader &shader, std::vector<char> &data);

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
                    vkDestroyShaderModule(device, vertShader.shaderModule, nullptr);
                }

                if (fragShader.shaderModule)
                {
                    vkDestroyShaderModule(device, fragShader.shaderModule, nullptr);
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
                vkDestroyShaderModule(device, vertShader.shaderModule, nullptr);
            }

            if (fragShader.shaderModule)
            {
                vkDestroyShaderModule(device, fragShader.shaderModule, nullptr);
            }
        }
    };

} // namespace sas