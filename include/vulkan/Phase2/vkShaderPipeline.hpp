#pragma once

#include "volk.h"

#include "vkDevice.hpp"
#include "vkDescriptor.hpp"

#include <vector>
#include <span>

#include <iostream>

namespace sas
{
    struct VulkanShaderConfig
    {
        VkShaderStageFlagBits stage;
        std::span<const uint32_t> spirvCode;

        const char *entryPoint = "main";
        const VkSpecializationInfo *specializationInfo = nullptr;

        VkDescriptorSetLayout descriptor;
        // Specifically needed for VulkanDynamicShader (Shader Objects)
        // std::span<const VkDescriptorSetLayout> setLayouts = {};
        // std::span<const VkPushConstantRange> pushConstants = {};

        VkShaderStageFlagBits nextStage = static_cast<VkShaderStageFlagBits>(0);
    };

    class VulkanShader
    {
    private:
        VulkanDevice &device;
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageInfo{};

    public:
        VulkanShader(VulkanDevice &dev, const VulkanShaderConfig& config);

        [[nodiscard]]VkShaderModule getShaderModule() const noexcept
        {
            return shaderModule;
        }

        [[nodiscard]]VkPipelineShaderStageCreateInfo getShaderInfo() const noexcept
        {
            return shaderStageInfo;
        }
    };

    class VulkanDynamicShader
    {
    private:
        VulkanDevice &device;
        VkShaderEXT shaderModule;

    public:
        VulkanDynamicShader(VulkanDevice &dev, const VulkanShaderConfig& config);


        [[nodiscard]]VkShaderEXT getShaderModule() const noexcept
        {
            return shaderModule;
        }
    };

    template <typename T>
    concept ValidVulkanShader = std::same_as<T, VulkanShader> ||
                                std::same_as<T, VulkanDynamicShader>;

    template <ValidVulkanShader ShaderTemplate>
    class VulkanPipeline
    {
    private:
        VulkanDevice &device;
        VulkanDescriptor &descriptor;

        ShaderTemplate vertShader;
        ShaderTemplate fragShader;

    public:
        VulkanPipeline(VulkanDevice &dev, VulkanDescriptor &desc, const std::vector<uint32_t>& vertCode, const std::vector<uint32_t>& fragCode) noexcept;

        VulkanPipeline(const VulkanPipeline &) = delete;
        VulkanPipeline &operator=(const VulkanPipeline &) = delete;
        VulkanPipeline(VulkanPipeline &&) noexcept = default;
        VulkanPipeline &operator=(VulkanPipeline &&) noexcept = default;
        ~VulkanPipeline() = default;

        [[nodiscard]] std::pair<ShaderTemplate, ShaderTemplate> getShaderStages() const noexcept
        {
            return std::make_pair(vertShader, fragShader);
        }
    };

    using VulkanShaderPipeline = VulkanPipeline<VulkanShader>;
    using VulkanDynamicShaderPipeline = VulkanPipeline<VulkanDynamicShader>;
} // namespace sas