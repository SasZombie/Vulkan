#pragma once

#include "vkComponents.hpp"
#include "Window.hpp"
#include "Logger.hpp"
#include "CommandBus.hpp"

#include <memory>
#include <vector>

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

    class IEditorPannel
    {
    public:
        EditorContext context;

        virtual void renderUI() const noexcept = 0;

        virtual ~IEditorPannel() noexcept = default;
    };

    class EditorPannel final : public IEditorPannel
    {
    public:
        CommandBus &commandBus;
        EditorPannel(CommandBus &comBus) noexcept
            : commandBus(comBus)
        {
        }

        void renderUI() const noexcept override;
    };

    class FPSpannel final : public IEditorPannel
    {
    public:
        CommandBus &commandBus;
        FPSpannel(CommandBus &comBus) noexcept
            : commandBus(comBus)
        {
        }

        void renderUI() const noexcept override;
    };

    class EngineUi
    {
    private:
        VulkanDevice &device;
        VkDescriptorPool uiPool;
        std::vector<std::unique_ptr<IEditorPannel>> pannels;

        Logger *logger = BaseLogger::getLogger("Ui");

        void initPool();

        void newFrame() const noexcept;

        void writeFrames() const noexcept;

    public:
        EngineUi(VulkanDevices &vulkanDevice, const Window &window, CommandBus &bus) noexcept;

        void updateFrame() const noexcept;
        ~EngineUi() noexcept;
    };

} // namespace sas