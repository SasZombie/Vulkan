#include "AssetManager.hpp"

sas::MeshComponent sas::AssetManager::loadMesh(const std::string &path) noexcept
{
    uint32_t index;

    if (activeCount < slots.size())
    {
        index = firstFreeIndex;
        firstFreeIndex = slots[index].nextFreeIndex;
    }
    else
    {
        index = slots.size();
        slots.push_back(Slot{});
    }

    const std::vector<sas::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};


    const std::vector<int> indices = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };
    Mesh m1;
    m1.indices = indices;
    m1.vertices = vertices;
    m1.meshId = 67;

    slots[index].mesh = m1;

    slots[index].active = true;

    activeCount++;

    return {index, slots[index].generation};
}

void sas::AssetManager::unloadMesh(const MeshComponent &comp) noexcept
{
   if(!isValid(comp)) return;

    slots[comp.id].mesh; // unload resource

    slots[comp.id].active = false;
    slots[comp.id].nextFreeIndex = firstFreeIndex;
    firstFreeIndex = comp.id;

    slots[comp.id].generation++;

    activeCount--;
}

bool sas::AssetManager::isValid(const MeshComponent &handle) const noexcept
{
    
        if (handle.id >= slots.size())
            return false;
        if (!slots[handle.id].active)
            return false;
        if (slots[handle.id].generation != handle.generation)
            return false; 
        return true;
    
}

// std::shared_ptr<sas::Mesh> sas::AssetManager::loadMesh(const std::string &path, const std::shared_ptr<std::vector<Texture>> &tex) noexcept
// {
//     if (auto it = meshCache.find(path); it != meshCache.end())
//     {
//         if (auto existing = it->second.lock())
//             return existing;
//     }

//     // auto mesh = std::make_shared<Mesh>(loadObj(path, tex));
//     // meshCache[path] = mesh;

//     // return mesh;
// }

// std::shared_ptr<std::vector<sas::Texture>> sas::AssetManager::loadTexture(const std::string &path) noexcept
// {
//     if (auto it = textureCache.find(path); it != textureCache.end())
//     {
//         if (auto existing = it->second.lock())
//             return existing;
//     }

//     // auto tex = std::make_shared<std::vector<Texture>>(getDefaultTexture(path.c_str()));
//     // textureCache[path] = tex;

//     // return tex;
// }

// // std::shared_ptr<Shader> sas::AssetManager::loadShader(const std::string &vert, const std::string &frag)
// // {
// //     // Idealy we would separate vertex shaders and fragment shaders
// //     // But this will suffice for a demo
// //     const std::string key = vert + "|" + frag;

// //     if (auto it = shaderCache.find(key); it != shaderCache.end())
// //     {
// //         if (auto existing = it->second.lock())
// //             return existing;
// //     }

// //     auto shader = std::make_shared<Shader>(vert.c_str(), frag.c_str());
// //     shaderCache[key] = shader;
// //     return shader;
// // }

// // std::shared_ptr<sas::Asset> sas::AssetManager::createAsset(const std::shared_ptr<Shader> &shader, const std::shared_ptr<Mesh> &mesh, Window *window) noexcept
// // {
// //     const auto asset = std::make_shared<Asset>(shader, mesh, window);

// //     // Could work
// //     // asset->addNode(std::make_shared<UI>(window));

// //     return asset;
// // }
