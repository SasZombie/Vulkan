#pragma once
#include <vector>

#include "EntityRegistry.hpp"

namespace sas
{

    class Scene
    {
    public:
        uint32_t id;
        EntityRegistry sceneRegistry;
    };

    // struct SceneHash
    // {
    //     using is_transparent = void;
    //     size_t operator()(const Scene &s) const { return std::hash<int>{}(s.id); }
    //     size_t operator()(int id) const { return std::hash<int>{}(id); }
    // };

    // struct SceneEqual
    // {
    //     using is_transparent = void;
    //     bool operator()(const Scene &a, const Scene &b) const { return a.id == b.id; }
    //     bool operator()(const Scene &a, int id) const { return a.id == id; }
    // };

} // namespace sas
