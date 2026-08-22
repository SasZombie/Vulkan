#include "Material.hpp"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wcast-qual"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma GCC diagnostic pop


sas::Material* sas::MaterialManager::getMaterial(std::string_view name) noexcept
{

    const std::string matName = std::string(name);
    if(const auto& it = materialsCache.find(matName); it != materialsCache.end())
    {
        return &it->second;
    }

    return nullptr;
}

sas::VulkanDynamicShader &sas::MaterialManager::loadShader(const std::string &vert, const std::string &frag) noexcept
{
    auto key = std::make_pair(vert, frag);

    // Starting from 1 so ID = 0 means invalid
    static size_t shaderID = 1;

    logger->log("Loading Shaders: " + vert + " " + frag);

    auto [it, inserted] = shaderCache.try_emplace(
        std::move(key),
        shaderID,
        vulkanCtx.vkDevice,
        sharedObjs.vulkanDescriptor,
        vert,
        frag);

    ++shaderID;
    return it->second;
}

sas::Material *sas::MaterialManager::addMaterial(std::string_view name, VulkanDynamicShader &shader, RenderTexture &texture) noexcept
{   
    Material m;

    m.shader = &shader;
    m.albedoMap = texture;

    addTexture(m, texture);

    materialsCache[std::string(name)] = m;

    return &materialsCache.at(std::string(name));
}

sas::RenderTexture sas::MaterialManager::loadTexture(const std::string &path) noexcept
{  
    if (textureCache.contains(path))
    {
        return textureCache.at(path);
    }

    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels)
    {
        logger->warn("Cannot load texture " + path);

        pixels = stbi_load("resources/textures/textureNotFound.bmp", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    }

    if (!pixels)
    {
        logger->warn("Cannot load default texture. This might crash the program!");
        return {};
    }

    logger->log("Creating texture: " + path);

    VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    VmaAllocationInfo stagingAllocInfo;

    sharedObjs.allocator.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                stagingBuffer, stagingAllocation, &stagingAllocInfo);

    memcpy(stagingAllocInfo.pMappedData, pixels, static_cast<size_t>(imageSize));
    stbi_image_free(pixels);

    VkImage textureImage;
    VmaAllocation textureImageAllocation;

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

    sharedObjs.allocator.createImage(imageInfo, VMA_MEMORY_USAGE_AUTO, textureImage, textureImageAllocation);

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
    
    vmaDestroyBuffer(sharedObjs.allocator, stagingBuffer, stagingAllocation);

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
    t.allocation = textureImageAllocation;

    textureCache[path] = t;

    return t;
}

void sas::MaterialManager::addMaterial(std::string_view name, const Material& material) noexcept
{
    const std::string matName = std::string(name);
    if(materialsCache.contains(matName))
    {
        logger->warn("This matterial's name already exists, overwritting for now");
    }

    materialsCache[matName] = material;
}

void sas::MaterialManager::addTexture(Material &material, const RenderTexture &texture) noexcept
{
    material.descriptorSet = sharedObjs.vulkanDescriptor.allocateDescSet();
    if(material.descriptorSet == nullptr)
    {
        logger->error("Cannot allocate descriptor set\n");
        return;
    }

    VkDescriptorSet dstSet = material.descriptorSet;

    VkDescriptorImageInfo imageDescriptorInfo{};
    imageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageDescriptorInfo.imageView = texture.view;
    imageDescriptorInfo.sampler = sharedObjs.sampler.textureSampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = dstSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageDescriptorInfo;

    vkUpdateDescriptorSets(vulkanCtx.vkDevice, 1, &descriptorWrite, 0, nullptr);
}

// void sas::MaterialManager::addTexture(RenderObject &objWithMesh, const std::string &path) noexcept
// {
//     const auto &texture = loadTexture(path);
//     addTexture(objWithMesh, texture);
// }

