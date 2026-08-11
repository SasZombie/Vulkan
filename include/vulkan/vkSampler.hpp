#pragma once
#include "vkDevice.hpp"

namespace sas
{
    class VulkanSampler
    {
    private:
        VulkanDevice &device;
    public:
        VkSampler textureSampler;

        VulkanSampler(VulkanDevice& dev) noexcept;
        ~VulkanSampler() noexcept
        {
           vkDestroySampler(device, textureSampler, nullptr); 
        }
    };

} // namespace sas
