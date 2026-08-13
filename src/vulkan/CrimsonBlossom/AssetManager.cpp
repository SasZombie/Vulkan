#include "AssetManager.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wcast-qual"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma GCC diagnostic pop

#include <cstring>
#include <fstream>
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
    std::vector<sas::Vertex> vertices;
    std::vector<int> indices;

    // Reading Obj file
    std::ifstream file(std::string(filename), std::ios::in | std::ios::binary);
    if (!file)
    {
        std::cerr << "Model not found " << filename << '\n';
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

    std::cout << "Loading: object " << filename << '\n';

    return {vertices, indices};
}

// TODO: add .gltf format support
// TODO: Here some kind of dispatcher for diff formats :D and checkers for real files
sas::Mesh sas::AssetManager::getRawMesh(std::string_view path) const noexcept
{
    return loadObj(path);
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
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // 1. Create & Copy Vertex Buffer
    createBuffer(sizeof(mesh.vertices[0]) * mesh.vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 vertexBuffer, vertexBufferMemory);

    void *data;
    vkMapMemory(vulkanCtx.vkDevice, vertexBufferMemory, 0, sizeof(mesh.vertices[0]) * mesh.vertices.size(), 0, &data);
    memcpy(data, mesh.vertices.data(), (size_t)sizeof(mesh.vertices[0]) * mesh.vertices.size());
    vkUnmapMemory(vulkanCtx.vkDevice, vertexBufferMemory);

    // 2. Create & Copy Index Buffer
    createBuffer(sizeof(mesh.indices[0]) * mesh.indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 indexBuffer, indexBufferMemory);

    vkMapMemory(vulkanCtx.vkDevice, indexBufferMemory, 0, sizeof(mesh.indices[0]) * mesh.indices.size(), 0, &data);
    memcpy(data, mesh.indices.data(), (size_t)sizeof(mesh.indices[0]) * mesh.indices.size());
    vkUnmapMemory(vulkanCtx.vkDevice, indexBufferMemory);

    return {vertexBuffer, indexBuffer, vertexBufferMemory, indexBufferMemory, mesh.indices.size()};
}

sas::AssetManager::AssetManager(VulkanDevices &ctx, VulkanSharedObjects &shardObj) noexcept
    : vulkanCtx(ctx), sharedObjs(shardObj), sampler(vulkanCtx.vkDevice)
{
    loadTexture(defaulTexturePath);
}

sas::RenderMesh sas::AssetManager::loadMesh(const std::string &path) noexcept
{
    if (meshCache.contains(path))
    {
        return meshCache.at(path);
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

    meshCache[path] = gpuObject;

    return gpuObject;
}

sas::RenderTexture sas::AssetManager::loadTexture(const std::string &path) noexcept
{

    if (textureCache.contains(path))
    {
        return textureCache.at(path);
    }

    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels)
    {
        std::cerr << "[[Warning]]! Cannot load texture " << path << '\n';
        pixels = stbi_load("resources/textures/textureNotFound.bmp", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    }

    if (!pixels)
    {
        std::cerr << "[[Warning]]! Cannot load default texture. This might crash the program!\n";
        return {};
    }

    std::cout << "Loading: texture " << path << '\n';

    VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(vulkanCtx.vkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(vulkanCtx.vkDevice, stagingBufferMemory);
    stbi_image_free(pixels);

    // 2. Create GPU VkImage
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(texWidth);
    imageInfo.extent.height = static_cast<uint32_t>(texHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateImage(vulkanCtx.vkDevice, &imageInfo, nullptr, &textureImage);

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(vulkanCtx.vkDevice, textureImage, &memReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkAllocateMemory(vulkanCtx.vkDevice, &allocInfo, nullptr, &textureImageMemory);
    vkBindImageMemory(vulkanCtx.vkDevice, textureImage, textureImageMemory, 0);

    VkCommandBuffer cmd = vulkanCtx.vkCommand.getCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &beginInfo);

    // Barrier 1: UNDEFINED -> TRANSFER_DST_OPTIMAL
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = textureImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    // Copy buffer to image
    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1};

    vkCmdCopyBufferToImage(cmd, stagingBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    // Barrier 2: TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkEndCommandBuffer(cmd);

    // Submit the command buffer to the GPU
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(vulkanCtx.vkDevice.getQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkanCtx.vkDevice.getQueue());

    vkDestroyBuffer(vulkanCtx.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(vulkanCtx.vkDevice, stagingBufferMemory, nullptr);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView textureImageView;
    vkCreateImageView(vulkanCtx.vkDevice, &viewInfo, nullptr, &textureImageView);

    RenderTexture t;
    t.image = textureImage;
    t.view = textureImageView;
    t.memory = textureImageMemory;

    textureCache[path] = t;

    return t;
}

void sas::AssetManager::addTexture(RenderObject &objWithMesh, const std::string &path) noexcept
{

    const auto &texture = loadTexture(path);

    addTexture(objWithMesh, texture);
}

void sas::AssetManager::addTexture(RenderObject &objWithMesh, const RenderTexture &texture) noexcept
{
    objWithMesh.descriptorSet = sharedObjs.shaderDescriptor.allocateDescSet();
    VkDescriptorSet dstSet = objWithMesh.descriptorSet;

    VkDescriptorImageInfo imageDescriptorInfo{};
    imageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageDescriptorInfo.imageView = texture.view;
    imageDescriptorInfo.sampler = sampler.textureSampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = dstSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageDescriptorInfo;

    vkUpdateDescriptorSets(vulkanCtx.vkDevice, 1, &descriptorWrite, 0, nullptr);
}
