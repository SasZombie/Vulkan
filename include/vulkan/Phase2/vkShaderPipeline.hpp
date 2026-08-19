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

        const char *entryPoint = "main";
        const VkSpecializationInfo *specializationInfo = nullptr;
        VkDescriptorSetLayout descriptor;

        VkShaderStageFlagBits nextStage = static_cast<VkShaderStageFlagBits>(0);
    };

    class VulkanDynamicShader
    {
    private:
        uint32_t id;
        VulkanDevice &device;
        VkShaderEXT vertexShader;
        VkShaderEXT fragmentShader;

        void populateShader(VkShaderEXT &shader, const VulkanShaderConfig &config, const std::vector<uint32_t> &codeData) noexcept;

    public:
        VulkanDynamicShader(uint32_t shaderId, VulkanDevice &dev, const VulkanDescriptor &desc, const std::string& vert, const std::string& frag);

        VulkanDynamicShader(const VulkanDynamicShader &) = delete;
        VulkanDynamicShader &operator=(const VulkanDynamicShader &) = delete;

        VulkanDynamicShader(VulkanDynamicShader &&other) noexcept
            : id(other.id), device(other.device), vertexShader(other.vertexShader), fragmentShader(other.fragmentShader)
        {
            other.vertexShader = nullptr;
            other.fragmentShader = nullptr;
        }

        VulkanDynamicShader &operator=(VulkanDynamicShader &&other) noexcept
        {
            if (this != &other)
            {
                if (vertexShader != VK_NULL_HANDLE)
                    vkDestroyShaderEXT(device, vertexShader, nullptr);
                if (fragmentShader != VK_NULL_HANDLE)
                    vkDestroyShaderEXT(device, fragmentShader, nullptr);

                vertexShader = other.vertexShader;
                fragmentShader = other.fragmentShader;
                
                other.vertexShader = VK_NULL_HANDLE;
                other.fragmentShader = VK_NULL_HANDLE;
            }
            return *this;
        }

        [[nodiscard]] uint32_t getId() const noexcept
        {
            return id;
        }

        ~VulkanDynamicShader()
        {
            if (vertexShader != VK_NULL_HANDLE)
                vkDestroyShaderEXT(device, vertexShader, nullptr);

            if (fragmentShader != VK_NULL_HANDLE)
                vkDestroyShaderEXT(device, fragmentShader, nullptr);
        }

        [[nodiscard]] std::pair<VkShaderEXT, VkShaderEXT> getShaderModule() const noexcept
        {
            return std::make_pair(vertexShader, fragmentShader);
        }
    };
} // namespace sas