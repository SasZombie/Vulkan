#include "AssetManager.hpp"

#include <cstring>
#include <fstream>
#include <ranges>
#include "Mesh.hpp"
#include "Math.hpp"

static const std::string defaulTexturePath = "resources/textures/textureNotFound.bmp";

static float _stringToFloat(const std::string &source) noexcept
{
    std::stringstream ss(source.c_str());
    float result;
    ss >> result;
    return result;
}

[[maybe_unused]] static unsigned int _stringToUint(const std::string &source) noexcept
{
    std::stringstream ss(source.c_str());
    unsigned int result;
    ss >> result;
    return result;
}

static int _stringToInt(const std::string &source) noexcept
{
    std::stringstream ss(source.c_str());
    int result;
    ss >> result;
    return result;
}

static void _stringTokenize(const std::string &source, std::vector<std::string> &tokens) noexcept
{
    tokens.clear();
    std::string aux = source;
    for (unsigned int i = 0; i < aux.size(); i++)
        if (aux[i] == '\t' || aux[i] == '\n')
            aux[i] = ' ';
    std::stringstream ss(aux, std::ios::in);
    while (ss.good())
    {
        std::string s;
        ss >> s;
        if (s.size() > 0)
            tokens.push_back(s);
    }
}

static void _faceTokenize(const std::string &source, std::vector<std::string> &tokens) noexcept
{
    std::string aux = source;
    for (unsigned int i = 0; i < aux.size(); i++)
        if (aux[i] == '\\' || aux[i] == '/')
            aux[i] = ' ';
    _stringTokenize(aux, tokens);
}

[[nodiscard]] static sas::Mesh loadObj(std::string_view filename) noexcept
{
    const auto &logger = sas::BaseLogger::getLogger("Asset");

    std::vector<sas::Vertex> vertices;
    std::vector<int> indices;

    const std::string strFile{filename};
    // Reading Obj file
    std::ifstream file(strFile, std::ios::in | std::ios::binary);
    if (!file)
    {
        logger->warn("Model not found " + std::string(filename));
        return {};
    }

    std::string line;
    std::vector<std::string> tokens, facetokens;

    std::vector<sas::math::Vec3> positions;
    positions.reserve(1000);

    std::vector<sas::math::Vec3> normals;
    normals.reserve(1000);

    std::vector<sas::math::Vec2> texcoords;
    texcoords.reserve(1000);

    // Parsing obj file
    while (std::getline(file, line))
    {
        _stringTokenize(line, tokens);

        if (tokens.size() == 0)
            continue;

        // Comments
        if (tokens.size() > 0 && tokens[0].at(0) == '#')
            continue;

        // Vertices
        if (tokens.size() > 3 && tokens[0] == "v")
            positions.push_back(sas::math::Vec3(_stringToFloat(tokens[1]), _stringToFloat(tokens[2]), _stringToFloat(tokens[3])));

        // Normals
        if (tokens.size() > 3 && tokens[0] == "vn")
            normals.push_back(sas::math::Vec3(_stringToFloat(tokens[1]), _stringToFloat(tokens[2]), _stringToFloat(tokens[3])));

        // Texture Coords
        if (tokens.size() > 2 && tokens[0] == "vt")
            texcoords.push_back(sas::math::Vec2(_stringToFloat(tokens[1]), _stringToFloat(tokens[2])));

        // Faces
        if (tokens.size() >= 4 && tokens[0] == "f")
        {
            unsigned int face_format = 0;
            if (tokens[1].find("//") != std::string::npos)
                face_format = 3;
            _faceTokenize(tokens[1], facetokens);

            if (facetokens.size() == 3)
                face_format = 4;
            else
            {
                if (facetokens.size() == 2)
                {
                    if (face_format != 3)
                        face_format = 2;
                }
                else
                {
                    face_format = 1;
                }
            }

            unsigned int index_of_first_vertex_of_face = -1;

            for (unsigned int num_token = 1; num_token < tokens.size(); num_token++)
            {
                sas::Vertex vertex;

                if (tokens[num_token].at(0) == '#')
                    break;
                _faceTokenize(tokens[num_token], facetokens);

                if (face_format == 1) // Just pos
                {
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};

                    vertices.push_back(vertex);
                }
                else if (face_format == 2) // Pos and texcoords
                {
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    int t_index = _stringToInt(facetokens[1]);
                    if (t_index > 0)
                        t_index -= 1;
                    else
                        t_index = texcoords.size() + t_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};
                    vertex.texCoord = {texcoords[t_index].x, texcoords[t_index].y};
                    vertices.push_back(vertex);
                }
                else if (face_format == 3)
                {
                    // Pos and normal
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    int n_index = _stringToInt(facetokens[1]);
                    if (n_index > 0)
                        n_index -= 1;
                    else
                        n_index = normals.size() + n_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};
                    vertex.normals = {normals[n_index].x, normals[n_index].y, normals[n_index].z};

                    vertices.push_back(vertex);
                }
                else
                {
                    // Normal and texcoord
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    int t_index = _stringToInt(facetokens[1]);
                    if (t_index > 0)
                        t_index -= 1;
                    else
                        t_index = normals.size() + t_index;

                    int n_index = _stringToInt(facetokens[2]);
                    if (n_index > 0)
                        n_index -= 1;
                    else
                        n_index = normals.size() + n_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};
                    vertex.normals = {normals[n_index].x, normals[n_index].y, normals[n_index].z};
                    vertex.texCoord = {texcoords[t_index].x, texcoords[t_index].y};

                    vertices.push_back(vertex);
                }

                if (num_token < 4)
                {
                    if (num_token == 1)
                        index_of_first_vertex_of_face = vertices.size() - 1;

                    indices.push_back(vertices.size() - 1);
                }
                else
                {
                    indices.push_back(index_of_first_vertex_of_face);
                    indices.push_back(vertices.size() - 2);
                    indices.push_back(vertices.size() - 1);
                }
            }
        }
    }

    logger->log("Loading: object " + std::string(filename));

    return {strFile, "placeholder", vertices, indices};
}

static void copyBuffer(const sas::VulkanDevices &devs, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) noexcept
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = devs.vkCommand.getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(devs.vkDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(devs.vkDevice.getQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(devs.vkDevice.getQueue());

    vkFreeCommandBuffers(devs.vkDevice, devs.vkCommand.getCommandPool(), 1, &commandBuffer);
}

sas::AssetManager::AssetManager(VulkanDevices &ctx, VulkanSharedObjects &shardObj, CommandBus &comBus) noexcept
    : vulkanCtx(ctx), sharedObjs(shardObj), materialManager(vulkanCtx, sharedObjs, comBus), bus(comBus)
{
    materialManager.loadTexture(defaulTexturePath);

    bus.subscribe<QuerryMapCommand<std::string, Mesh>>([&](const QuerryMapCommand<std::string, Mesh> &cmd)
                                                       { cmd.map = &this->cpuMeshCache; });

    bus.subscribe<CreateNewMeshCommand>([this](const CreateNewMeshCommand &cmd)
                                        { loadMesh(cmd.meshPath); });

    bus.subscribe<CreateRenderMeshCommand>([this](const CreateRenderMeshCommand &cmd) {
        cmd.renderMesh = loadMesh(cmd.renderMeshPath); 
    });
}

// TODO: add .gltf format support
// TODO: Here some kind of dispatcher for diff formats :D and checkers for real files
sas::Mesh sas::AssetManager::getRawMesh(std::string_view path) noexcept
{
    const std::string strPath{path};
    auto it = cpuMeshCache.find(strPath);

    if (it != cpuMeshCache.end())
    {
        return it->second;
    }

    Mesh m = loadObj(path);
    auto [insertedIt, success] = cpuMeshCache.emplace(strPath, std::move(m));

    return insertedIt->second;
}

uint32_t sas::AssetManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanCtx.vkPhysical, &memProperties);

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

void sas::AssetManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const noexcept
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(vulkanCtx.vkDevice, &bufferInfo, nullptr, &buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanCtx.vkDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(vulkanCtx.vkDevice, &allocInfo, nullptr, &bufferMemory);
    vkBindBufferMemory(vulkanCtx.vkDevice, buffer, bufferMemory, 0);
}

sas::RenderMesh sas::AssetManager::createGpuMesh(const sas::Mesh &mesh) const noexcept
{
    VkDeviceSize vertexSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();
    VkDeviceSize indexSize = sizeof(mesh.indices[0]) * mesh.indices.size();

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    VmaAllocationInfo stagingAllocInfo;

    VkBuffer vertexBuffer, indexBuffer;
    VmaAllocation vertexAllocation, indexAllocation;

    sharedObjs.allocator.createBuffer(vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                                      stagingBuffer, stagingAllocation, &stagingAllocInfo);

    memcpy(stagingAllocInfo.pMappedData, mesh.vertices.data(), (size_t)vertexSize);

    sharedObjs.allocator.createBuffer(vertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      0,
                                      vertexBuffer, vertexAllocation);

    copyBuffer(vulkanCtx, stagingBuffer, vertexBuffer, vertexSize);

    vmaDestroyBuffer(sharedObjs.allocator, stagingBuffer, stagingAllocation);

    sharedObjs.allocator.createBuffer(indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                                      stagingBuffer, stagingAllocation, &stagingAllocInfo);

    memcpy(stagingAllocInfo.pMappedData, mesh.indices.data(), (size_t)indexSize);

    sharedObjs.allocator.createBuffer(indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                      0,
                                      indexBuffer, indexAllocation);

    copyBuffer(vulkanCtx, stagingBuffer, indexBuffer, indexSize);
    vmaDestroyBuffer(sharedObjs.allocator, stagingBuffer, stagingAllocation);

    return {vertexBuffer, indexBuffer, vertexAllocation, indexAllocation, mesh.indices.size()};
}

sas::RenderMesh* sas::AssetManager::loadMesh(const std::string &path) noexcept
{
    if (gpuMeshCache.contains(path))
    {
        return &gpuMeshCache.at(path);
    }

    const auto &loadedMesh = getRawMesh(path);

    RenderMesh gpuObject = createGpuMesh(loadedMesh);
    VkBuffer buffers[] = {gpuObject.vertexBuffer};

    VkDeviceSize offsets[] = {0};

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(vulkanCtx.vkCommand.getCommandBuffer(), &beginInfo);

    vkCmdBindVertexBuffers(vulkanCtx.vkCommand.getCommandBuffer(), 0, 1, buffers, offsets);

    vkCmdBindIndexBuffer(vulkanCtx.vkCommand.getCommandBuffer(), gpuObject.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkEndCommandBuffer(vulkanCtx.vkCommand.getCommandBuffer());

    gpuMeshCache[path] = gpuObject;

    return &gpuMeshCache[path];
}
