#include "vkDescriptor.hpp"

sas::VulkanDescriptor::VulkanDescriptor(VulkanDevice &dev) noexcept
    : device(dev)
{
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

    VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};
    VkDescriptorPoolCreateInfo poolInfo{
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, 0, 1, 1, &poolSize};

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
