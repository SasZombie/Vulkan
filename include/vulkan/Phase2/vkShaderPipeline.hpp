#pragma once

#include "volk.h"

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


        VkDescriptorSet descriptorSet;
        VkDescriptorSetLayout descriptorSetLayout;

        void populateShader(VulkanShader &shader, std::vector<char> &data);

    public:
        VulkanShaderPipeline(VulkanDevice &vulkanDevice);

        [[nodiscard]] std::pair<VulkanShader, VulkanShader> getShaderStages() const noexcept
        {
            return std::make_pair(vertShader, fragShader);
        }

        [[nodiscard]] std::pair<VkDescriptorSet, VkDescriptorSetLayout> getShaderDescriptor() const noexcept
        {
            return std::make_pair(descriptorSet, descriptorSetLayout);
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

    struct VulkanDynamicShader
    {
        VkShaderEXT shaderModule;
        VkShaderCreateInfoEXT shaderStageInfo{};
    };

    class VulkanDynamicShaderPipeline
    {
    private:
        VulkanDevice &device;
        VulkanDynamicShader vertShader;
        VulkanDynamicShader fragShader;

        VkDescriptorSet descriptorSet;
        VkDescriptorSetLayout descriptorSetLayout;

    public:
        VulkanDynamicShaderPipeline(VulkanDevice &vulkanDevice);

        [[nodiscard]] std::pair<VkDescriptorSet, VkDescriptorSetLayout> getShaderDescriptor() const noexcept
        {
            return std::make_pair(descriptorSet, descriptorSetLayout);
        }
        [[nodiscard]] std::pair<VulkanDynamicShader, VulkanDynamicShader> getShaderStages() const noexcept
        {
            return std::make_pair(vertShader, fragShader);
        }
        VulkanDynamicShaderPipeline(const VulkanDynamicShaderPipeline &) = delete;
        VulkanDynamicShaderPipeline &operator=(const VulkanDynamicShaderPipeline &) = delete;

        VulkanDynamicShaderPipeline(VulkanDynamicShaderPipeline &&other) noexcept
            : device(other.device), vertShader(other.vertShader), fragShader(other.fragShader)
        {
            other.fragShader.shaderModule = nullptr;
            other.vertShader.shaderModule = nullptr;
        }

        VulkanDynamicShaderPipeline &operator=(VulkanDynamicShaderPipeline &&other) noexcept
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

        ~VulkanDynamicShaderPipeline()
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