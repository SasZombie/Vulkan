#pragma once

#include "vkComponents.hpp"
#include "Window.hpp"

namespace sas
{

    struct Scene;
    struct EditorContext
    {
        Scene *activeScene = nullptr;
        uint32_t selectedEntityId = 0; 
        bool isEntitySelected = false;

        void selectEntity(uint32_t id) noexcept
        {
            selectedEntityId = id;
            isEntitySelected = true;
        }

        void clearSelection() noexcept
        {
            selectedEntityId = 0;
            isEntitySelected = false;
        }
    };

    

    class EngineUi
    {
    private:
        VulkanDevice &device;
        VkDescriptorPool uiPool;

        void initPool();

        void newFrame() const noexcept;

        void writeFrames() const noexcept;

    public:
        EngineUi(VulkanDevices &vulkanDevice, const Window &window) noexcept;

        void updateFrame() const noexcept;
        ~EngineUi() noexcept;
    };

} // namespace sas