#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>

#include "Math.hpp"

namespace sas
{

    struct Vertex
    {
        math::Vec3 pos;
        math::Vec3 color;
        math::Vec2 texCoord;
    };

    struct Texture
    {
        unsigned int id;
        std::string type;
    };

    struct MeshComponent
    {
        uint32_t id;
        uint32_t generation;
    };

    class Mesh
    {
    public:
        uint32_t meshId = 0;
        std::vector<Vertex> vertices;
        std::vector<int> indices;

        // std::shared_ptr<std::vector<Texture>> textures;

    //     Mesh() noexcept = default;
        // Mesh(std::vector<Vertex> vertices, std::vector<int> indices, std::shared_ptr<std::vector<Texture>> textures) noexcept;
        // Mesh(std::vector<Vertex> vertices, std::vector<int> indices, const std::filesystem::path &pth) noexcept;
    //     ~Mesh() noexcept = default;

    //     void setTextures(std::shared_ptr<std::vector<Texture>> textures) noexcept;

    //     friend std::ostream &operator<<(std::ostream &os, const Mesh &mesh);

    // private:
    //     void setup2() noexcept;
    //     void setup() noexcept;
    };

} // namespace sas