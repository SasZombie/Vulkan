#pragma once

#include "AssetManager.hpp"
#include "vkRenderer.hpp"
#include "Scene.hpp"
#include "EngineUi.hpp"
#include "Logger.hpp"
#include <unordered_map>
#include <ranges>

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
        std::unordered_map<uint32_t, Scene> scenes;

        uint32_t activeSceneId = std::numeric_limits<uint32_t>::max();

        Logger* logger = BaseLogger::getLogger("Engine");

        CrimsonBlossom(Window &window, Camera& camera) noexcept
            : vulkanLowLvl(window), sharedObjects(vulkanLowLvl), 
              engineUi(vulkanLowLvl, window),
              vkRenderer(window, camera, vulkanLowLvl, sharedObjects), 
              assetManager(vulkanLowLvl, sharedObjects)
        {
        }

        Scene* createScene() noexcept
        {
            static uint32_t currentSceneId = 0;

            const auto& [iter, success] = scenes.try_emplace(currentSceneId, currentSceneId);

            if(!success)
            {
                logger->error("Cannot add a new scene");
                return nullptr;
            }

            ++currentSceneId;

            return &iter->second; 
        }

        [[nodiscard]]std::vector<Scene> getScenes() const noexcept
        {
            return scenes | std::views::values | std::ranges::to<std::vector>();
        }

        void update() noexcept;

        void createUi() const noexcept;
    };    
    
} // namespace sas
