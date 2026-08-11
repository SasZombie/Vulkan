#pragma once

#include "volk.h"

#include "vkDevice.hpp"
#include "vkDescriptor.hpp"

#include <vector>
#include <iostream>

namespace sas
{
    struct VulkanShader
    {
        VkShaderModule shaderModule;
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
    };

    struct VulkanDynamicShader
    {
        VkShaderEXT shaderModule;
        VkShaderCreateInfoEXT shaderStageInfo{};
    };

    template <typename ShaderStruct>
    class ManagedShader
    {
    public:
        ManagedShader() noexcept = default;
        ManagedShader(VkDevice ndevice, ShaderStruct nshader) noexcept
            : device(ndevice), shader(nshader) {}

        ~ManagedShader()
        {
            cleanup();
        }

        ManagedShader(const ManagedShader &) = delete;
        ManagedShader &operator=(const ManagedShader &) = delete;

        ManagedShader(ManagedShader &&other) noexcept
            : device(other.device), shader(other.shader)
        {
            other.shader.shaderModule = VK_NULL_HANDLE;
            other.device = VK_NULL_HANDLE;
        }

        ManagedShader &operator=(ManagedShader &&other) noexcept
        {
            if (this != &other)
            {
                cleanup();
                device = other.device;
                shader = other.shader;
                other.shader.shaderModule = nullptr;
                other.device = VK_NULL_HANDLE;
            }
            return *this;
        }

        [[nodiscard]] const ShaderStruct &get() const noexcept
        {
            return shader;
        }
        [[nodiscard]] ShaderStruct &get() noexcept
        {
            return shader;
        }

    private:
        void cleanup() noexcept
        {            
            std::cout << "Shader cleanup clled \n";
            
            if (device != VK_NULL_HANDLE)
            {
                std::cout << "Shader cleanup clled and not with null device \n";

                if constexpr (std::is_same_v<ShaderStruct, VulkanShader>)
                {
                    vkDestroyShaderModule(device, shader.shaderModule, nullptr);
                }
                else if constexpr (std::is_same_v<ShaderStruct, VulkanDynamicShader>)
                {
                    vkDestroyShaderEXT(device, shader.shaderModule, nullptr);
                }
                else
                {
                    std::cout << "Shader cleanup clled and not with unknwen shader \n";

                }
            }
        }

        VkDevice device;
        ShaderStruct shader{};
    };

    template <typename ShaderType>
    class GenericVulkanPipeline
    {
    public:
        using Shader = ShaderType;

        GenericVulkanPipeline(VulkanDevice &vulkanDevice, VulkanDescriptor& desc) noexcept;

        GenericVulkanPipeline(const GenericVulkanPipeline &) = delete;
        GenericVulkanPipeline &operator=(const GenericVulkanPipeline &) = delete;
        GenericVulkanPipeline(GenericVulkanPipeline &&) noexcept = default;
        GenericVulkanPipeline &operator=(GenericVulkanPipeline &&) noexcept = default;
        ~GenericVulkanPipeline() = default;


        [[nodiscard]] std::pair<ShaderType, ShaderType> getShaderStages() const noexcept
        {
            return std::make_pair(vertShader.get(), fragShader.get());
        }

    private:
        VulkanDevice &device;
        VulkanDescriptor& descriptor;
        ManagedShader<ShaderType> vertShader;
        ManagedShader<ShaderType> fragShader;

        void createShaders() noexcept;
    };

    using VulkanShaderPipeline = GenericVulkanPipeline<VulkanShader>;
    using VulkanDynamicShaderPipeline = GenericVulkanPipeline<VulkanDynamicShader>;

} // namespace sas