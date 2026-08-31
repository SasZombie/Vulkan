#pragma once

#include "vkDescriptor.hpp"
#include "vkVulkan.hpp"
#include "Mesh.hpp"
#include "Math.hpp"
#include "Logger.hpp"
#include "vkComponents.hpp"
#include "CommandBus.hpp"

#include <unordered_map>

namespace sas
{

    struct MaterialProperties
    {
        math::Vec4 baseColorFactor;
        float metalicFactor;
        float roughnessFactor;
        math::Vec2 textureTiling;
    };

    struct RenderState
    {
        bool isTransparent = false;
        bool doubleSided = false;
    };

    struct Material
    {
        std::string name;
        VulkanDynamicShader *shader;

        RenderTexture albedoMap; // Base color
        // RenderTexture normalMap; // Details/bumps
        // RenderTexture ormMap;    // Occlusion, Roughness, Metallic packed into RGB channels

        MaterialProperties properties;
        VkDescriptorSet descriptorSet;

        RenderState state;

        // Here builder pattern
    };

    class MaterialManager
    {
    private:
        struct PairHash
        {
            std::size_t operator()(const std::pair<std::string, std::string> &p) const noexcept
            {
                std::size_t h1 = std::hash<std::string>{}(p.first);
                std::size_t h2 = std::hash<std::string>{}(p.second);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            }
        };

        std::unordered_map<std::string, Material> materialsCache;
        std::unordered_map<std::string, RenderTexture> textureCache;
        std::unordered_map<std::pair<std::string, std::string>, VulkanDynamicShader, PairHash> shaderCache;

        VulkanDevices &vulkanCtx;
        VulkanSharedObjects &sharedObjs;
        CommandBus &bus;

        Logger *logger = BaseLogger::getLogger("Material");

        void addTexture(Material &material, const RenderTexture &texture) noexcept;

    public:
        MaterialManager(VulkanDevices &vctx, VulkanSharedObjects &sharedObj, CommandBus &cbus) noexcept
            : vulkanCtx(vctx), sharedObjs(sharedObj), bus(cbus)
        {
            bus.subscribe<QuerryMapCommand<std::string, Material>>([&](const QuerryMapCommand<std::string, Material> &cmd)
                                                               { cmd.map = &this->materialsCache; });
        }

        Material *getMaterial(std::string_view name) noexcept;
        void addMaterial(std::string_view name, const Material &material) noexcept;

        RenderTexture loadTexture(const std::string &path) noexcept;
        VulkanDynamicShader &loadShader(const std::string &vert = "", const std::string &frag = "") noexcept;

        // void addTexture(RenderObject &objWithMesh, const std::string &path) noexcept;

        // Material loadMaterial(std::string_view path) noexcept;
        Material *addMaterial(std::string_view name, VulkanDynamicShader &shader, RenderTexture &texture) noexcept;

        ~MaterialManager() noexcept
        {
            for (auto &elem : textureCache)
            {
                const auto &seccond = elem.second;

                if (seccond.allocation)
                {
                    vmaFreeMemory(sharedObjs.allocator, seccond.allocation);
                    ;
                }
                if (seccond.image)
                {
                    vkDestroyImage(vulkanCtx.vkDevice, seccond.image, nullptr);
                }

                if (seccond.view)
                {
                    vkDestroyImageView(vulkanCtx.vkDevice, seccond.view, nullptr);
                }
            }
        }
    };

} // namespace sas
