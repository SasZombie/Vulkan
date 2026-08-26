#pragma once

#include "AssetManager.hpp"
#include "vkRenderer.hpp"
#include "Scene.hpp"
#include "EngineUi.hpp"

namespace sas
{
    class CrimsonBlossom
    {

    private:
    public:
        VulkanDevices vulkanLowLvl;
        VulkanSharedObjects sharedObjects;

        EngineUi engineUi;
        VulkanRenderer vkRenderer;
        AssetManager assetManager;
        std::vector<Scene> scenes;

        CrimsonBlossom(Window &window, Camera& camera) noexcept
            : vulkanLowLvl(window), sharedObjects(vulkanLowLvl), 
              engineUi(vulkanLowLvl, window),
              vkRenderer(window, camera, vulkanLowLvl, sharedObjects), 
              assetManager(vulkanLowLvl, sharedObjects)
        {}

        void addScene(Scene newScene) noexcept
        {
            scenes.push_back(std::move(newScene));
        }

        std::vector<Scene> getScenes() const noexcept
        {
            return scenes;
        }

        void update() noexcept;
    };    
    
} // namespace sas
