#include "vkDescriptor.hpp"

//TODO: There needs to be a way to re-generate this descriptor
//Atm I hardcoded 1000 textures and 1000 descriptors
//But it should be dynamic
sas::VulkanDescriptor::VulkanDescriptor(VulkanDevice &dev) noexcept
    : device(dev)
{
    constexpr size_t hardcodedVal = 1000;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;

    vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);

    VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, hardcodedVal};
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.flags = 0;
    poolInfo.maxSets = hardcodedVal;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
}

VkDescriptorSet sas::VulkanDescriptor::allocateDescSet() noexcept
{
    VkDescriptorSet descSet;
    VkDescriptorSetAllocateInfo allocSetInfo{
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        descriptorPool,
        1,
        &descriptorSetLayout};
    vkAllocateDescriptorSets(device, &allocSetInfo, &descSet);

    return descSet;
}
