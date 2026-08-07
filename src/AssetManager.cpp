#include "AssetManager.hpp"

sas::Mesh getRawMesh(std::string_view path) noexcept
{
    const std::vector<sas::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

    const std::vector<int> indices = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
    };

    sas::Mesh m1;
    m1.indices = indices;
    m1.vertices = vertices;
    m1.meshId = 67;

    return m1;
}

// COPY PASTED PART
uint32_t findMemoryType(const VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        // Check 1: Is bit i set in typeFilter? (Is this memory type allowed for the buffer?)
        // Check 2: Does this memory type have all the property flags we requested?
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
// Helper function to create a buffer + allocate memory
void createBuffer(const VkDevice &device, const VkPhysicalDevice &vkPhysical, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    // Find memory type index matching CPU-visible flags from PhysicalDevice
    allocInfo.memoryTypeIndex = findMemoryType(vkPhysical, memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
#include <cstring>
std::pair<VkBuffer, VkBuffer> actualCreateBuffer(const sas::Mesh &mesh, const VkDevice &device, const VkPhysicalDevice &physicalDevice)
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // 1. Create & Copy Vertex Buffer
    createBuffer(device, physicalDevice, sizeof(mesh.vertices[0]) * mesh.vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 vertexBuffer, vertexBufferMemory);

    void *data;
    vkMapMemory(device, vertexBufferMemory, 0, sizeof(mesh.vertices[0]) * mesh.vertices.size(), 0, &data);
    memcpy(data, mesh.vertices.data(), (size_t)sizeof(mesh.vertices[0]) * mesh.vertices.size());
    vkUnmapMemory(device, vertexBufferMemory);

    // 2. Create & Copy Index Buffer
    createBuffer(device, physicalDevice, sizeof(mesh.indices[0]) * mesh.indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 indexBuffer, indexBufferMemory);

    vkMapMemory(device, indexBufferMemory, 0, sizeof(mesh.indices[0]) * mesh.indices.size(), 0, &data);
    memcpy(data, mesh.indices.data(), (size_t)sizeof(mesh.indices[0]) * mesh.indices.size());
    vkUnmapMemory(device, indexBufferMemory);

    return {vertexBuffer, indexBuffer};
}

sas::RenderObject sas::AssetManager::loadMesh(const std::string &path) noexcept
{
    const auto &loadedMesh = getRawMesh(path);

    const auto [vertex, index] = actualCreateBuffer(loadedMesh, vulkanCtx.vkDevice, vulkanCtx.vkPhysical);
    VkBuffer buffers[] = {vertex};

    VkDeviceSize offsets[] = {0};

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(vulkanCtx.vkCommand.getCommandBuffer(), &beginInfo);

    vkCmdBindVertexBuffers(vulkanCtx.vkCommand.getCommandBuffer(), 0, 1, buffers, offsets);

    vkCmdBindIndexBuffer(vulkanCtx.vkCommand.getCommandBuffer(), index, 0, VK_INDEX_TYPE_UINT32);

    vkEndCommandBuffer(vulkanCtx.vkCommand.getCommandBuffer());

    return
    {
        vertex, index, loadedMesh.indices.size()
    };
}

// sas::MeshComponent sas::AssetManager::loadMesh(const std::string &path) noexcept
// {
//     uint32_t index;

//     if (activeCount < slots.size())
//     {
//         index = firstFreeIndex;
//         firstFreeIndex = slots[index].nextFreeIndex;
//     }
//     else
//     {
//         index = slots.size();
//         slots.push_back(Slot{});
//     }

//     const std::vector<sas::Vertex> vertices = {
//         {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//         {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//         {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//         {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

//     const std::vector<int> indices = {
//         0, 1, 2, // First triangle
//         2, 3, 0  // Second triangle
//     };

//     Mesh m1;
//     m1.indices = indices;
//     m1.vertices = vertices;
//     m1.meshId = 67;

//     slots[index].mesh = m1;

//     slots[index].active = true;

//     activeCount++;

//     return {index, slots[index].generation};
// }

// void sas::AssetManager::unloadMesh(const MeshComponent &comp) noexcept
// {
//     if (!isValid(comp))
//         return;

//     slots[comp.id].mesh; // unload resource

//     slots[comp.id].active = false;
//     slots[comp.id].nextFreeIndex = firstFreeIndex;
//     firstFreeIndex = comp.id;

//     slots[comp.id].generation++;

//     activeCount--;
// }

// bool sas::AssetManager::isValid(const MeshComponent &handle) const noexcept
// {
//     if (handle.id >= slots.size())
//         return false;
//     if (!slots[handle.id].active)
//         return false;
//     if (slots[handle.id].generation != handle.generation)
//         return false;
//     return true;
// }

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
