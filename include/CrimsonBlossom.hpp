#pragma once

#include "AssetManager.hpp"
#include "vkRenderer.hpp"
#include "Scene.hpp"

namespace sas
{
    class CrimsonBlossom
    {
    public:
        VulkanDevices vulkanLowLvl;
        VulkanSharedObjects sharedObjects;
        VulkanRenderer vkRenderer;
        AssetManager assetManager;
        std::vector<Scene> scenes;

        // RenderObject createBuffer(const MeshComponent& component) const noexcept;

        CrimsonBlossom(Window &window, Camera& camera) noexcept
            : vulkanLowLvl(window), sharedObjects(vulkanLowLvl.vkDevice), vkRenderer(window, camera, vulkanLowLvl, sharedObjects), assetManager(vulkanLowLvl, sharedObjects)
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
