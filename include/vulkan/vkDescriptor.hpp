#pragma once

#include "vkDevice.hpp"

namespace sas
{

    class VulkanDescriptor
    {
    private:
        VulkanDevice& device;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;

    public:
        VulkanDescriptor(VulkanDevice& dev) noexcept;
        VkDescriptorSet allocateDescSet() noexcept;

        [[nodiscard]] VkDescriptorSetLayout getDescriptorLayout() const noexcept
        {
            return descriptorSetLayout;
        }
        ~VulkanDescriptor() noexcept
        {
           vkDestroyDescriptorPool(device, descriptorPool, nullptr); 
           vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        }
    };

} // namespace sas
