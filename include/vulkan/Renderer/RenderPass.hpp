#pragma once
#include <vector>
#include <string>
#include "EntityRegistry.hpp"
#include "vkComponents.hpp"
#include "vkMasterPipeline.hpp"
#include "Camera.hpp"

namespace sas
{

    struct RenderPassComponents
    {
        VulkanDevices& devices;
        VulkanMasterPipeline& masterPipe;
        Camera& camera;
    };

    class IRenderPass
    {
    protected:
        std::string name;
    public:
        bool isActive = true;
        
        IRenderPass(const std::string &passName) noexcept
            : name(passName)
        {
        }

        const std::string& getName() const noexcept
        {
            return name;
        }

        virtual void record(const std::vector<DrawingComponents> &objectsToRender, const RenderPassComponents& components) const noexcept = 0;

        virtual ~IRenderPass() = default;
    };

    class MainScenePass final : public IRenderPass
    {
    private:
        // void setUpRaster(const RenderObject& renderObj) const noexcept;
        // void setUpViewPort(const RenderObject& renderObj) const noexcept;
        void drawCallRecorder(const DrawingComponents& component, const RenderPassComponents& components) const noexcept;
    public:
        MainScenePass(const std::string &passName) noexcept
            : IRenderPass(passName)
        {
        }

        void record(const std::vector<DrawingComponents> &objectsToRender, const RenderPassComponents& components) const noexcept override;

        ~MainScenePass() noexcept = default;
    };

} // namespace sas