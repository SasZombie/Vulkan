#pragma once

#include "AssetManager.hpp"
#include "vkRenderer.hpp"
#include "Scene.hpp"

namespace sas
{
    class CrimsonBlossom
    {
    private:
    public:
        VulkanRenderer vkRenderer;
        AssetManager assetManager;
        std::vector<Scene> scenes;

        RenderObject createBuffer(const MeshComponent& component) const noexcept;

        CrimsonBlossom(Window &window, Camera& camera) noexcept
            : vkRenderer(window, camera)
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
