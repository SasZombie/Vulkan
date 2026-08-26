#pragma once

#include "vkComponents.hpp"
#include "Window.hpp"

namespace sas
{
    class EngineUi
    {
    private:
        VulkanDevice &device;
        VkDescriptorPool uiPool;

        void initPool();

    public:
        EngineUi(VulkanDevices &vulkanDevice, const Window &window) noexcept;
        ~EngineUi() noexcept;
    };

} // namespace sas