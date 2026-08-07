#include "CrimsonBlossom.hpp"

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
std::pair<VkBuffer, VkBuffer> actualCreateBuffer(const sas::Mesh& mesh, const VkDevice &device, const VkPhysicalDevice &physicalDevice)
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
// ^^^^^^^^6 COPY PASTED PART

sas::RenderObject sas::CrimsonBlossom::createBuffer(const MeshComponent& component) const noexcept
{

    const auto& mesh = assetManager.getMesh(component);
    const auto [vertex, index] = actualCreateBuffer(*mesh, vkRenderer.vkDevice, vkRenderer.vkPhysical);
    VkBuffer buffers[] = {vertex};

    VkDeviceSize offsets[] = {0};

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   
    vkBeginCommandBuffer(vkRenderer.vkCommand.getCommandBuffer(), &beginInfo);


    vkCmdBindVertexBuffers(vkRenderer.vkCommand.getCommandBuffer(), 0, 1, buffers, offsets);

    vkCmdBindIndexBuffer(vkRenderer.vkCommand.getCommandBuffer(), index, 0, VK_INDEX_TYPE_UINT32);

    vkEndCommandBuffer(vkRenderer.vkCommand.getCommandBuffer());


    return{
        vertex, index, mesh->indices.size() 
    };
}

void sas::CrimsonBlossom::update() noexcept
{

    const auto& allMeshComp = scenes[0].sceneRegistry.getComponents<RenderObject>();

    for(const auto& renderObject : allMeshComp->getData())
    {
        vkRenderer.drawFrame(renderObject);
    }
    

}