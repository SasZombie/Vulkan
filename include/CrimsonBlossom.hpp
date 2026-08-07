#pragma once

#include "AssetManager.hpp"
#include "vkRenderer.hpp"
#include "Scene.hpp"

namespace sas
{
    class CrimsonBlossom
    {
    public:
        VulkanLowLevel vulkanLowLvl;
        VulkanRenderer vkRenderer;
        AssetManager assetManager;
        std::vector<Scene> scenes;

        RenderObject createBuffer(const MeshComponent& component) const noexcept;

        CrimsonBlossom(Window &window, Camera& camera) noexcept
            : vulkanLowLvl(window), vkRenderer(window, camera, vulkanLowLvl), assetManager(vulkanLowLvl)
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
