#pragma once

#include "Math.hpp"
#include "vkVulkan.hpp"
#include "Mesh.hpp"

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

} // namespace sas