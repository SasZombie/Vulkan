#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include "Math.hpp"
#include "vkVulkan.hpp"

namespace sas
{

    // Cpu
    struct Vertex
    {
        math::Vec3 pos;
        math::Vec3 normals;
        math::Vec2 texCoord;
        math::Vec3 color;
    };

    //Cpu
    struct Texture
    {
        std::string path;
    };

    //Cpu
    struct Mesh
    {
        std::string path;
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<int> indices;
    };

    //GPU
    struct RenderTexture
    {
        VkImage image;
        VkImageView view;
        VmaAllocation allocation;
    };

    //GPU
    struct RenderMesh
    {
        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
        VmaAllocation vertexBufferMemory;
        VmaAllocation indexBufferMemory;
        size_t indexCount;
    };

  
} // namespace sas