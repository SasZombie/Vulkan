#include "vkShaderPipeline.hpp"
#include "vkComponents.hpp"

#include <vector>
#include <fstream>

static std::vector<uint32_t> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    const size_t fileSize = static_cast<size_t>(file.tellg());

    if (fileSize % sizeof(uint32_t) != 0) {
        throw std::runtime_error("SPIR-V file size is not a multiple of 4: " + filename);
    }

    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);

    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

sas::VulkanShader::VulkanShader(VulkanDevice &dev, const VulkanShaderConfig &config)
    : device(dev)
{

    VkShaderModuleCreateInfo createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = config.spirvCode.size_bytes();
    createInfo.pCode = config.spirvCode.data();

    vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

    // HERE CAN BE MORE STEPS IF NEEDED

    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = config.entryPoint;

    shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
}

sas::VulkanDynamicShader::VulkanDynamicShader(VulkanDevice &dev, const VulkanShaderConfig &config)
    : device(dev)
{

    VkPushConstantRange pushRange{};
    pushRange.stageFlags = config.stage;
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstants);

    const auto &descLay = config.descriptor;

    shaderStageInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    shaderStageInfo.stage = config.stage;
    shaderStageInfo.nextStage = config.nextStage;
    shaderStageInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    shaderStageInfo.codeSize = config.spirvCode.size();
    shaderStageInfo.pCode = config.spirvCode.data();
    shaderStageInfo.pName = config.entryPoint;
    shaderStageInfo.pushConstantRangeCount = 1;
    shaderStageInfo.pPushConstantRanges = &pushRange;
    shaderStageInfo.setLayoutCount = 1;
    shaderStageInfo.pSetLayouts = &descLay;

    vkCreateShadersEXT(device, 1, &shaderStageInfo, nullptr, &shaderModule);
}

template <sas::ValidVulkanShader ShaderTemplate>
sas::VulkanPipeline<ShaderTemplate>::VulkanPipeline(VulkanDevice &dev, VulkanDescriptor &desc, const std::vector<uint32_t>& vertCode, const std::vector<uint32_t>& fragCode) noexcept
    : device(dev), descriptor(desc), 

      vertShader(dev, VulkanShaderConfig{
                          .stage = VK_SHADER_STAGE_VERTEX_BIT,
                          .spirvCode = vertCode,
                          .descriptor = desc.getDescriptorLayout(),
                          .nextStage = VK_SHADER_STAGE_FRAGMENT_BIT}),

      fragShader(dev, VulkanShaderConfig{
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT, 
        .spirvCode = fragCode, 
        .descriptor = desc.getDescriptorLayout()})
{
}

template class sas::VulkanPipeline<sas::VulkanShader>;
template class sas::VulkanPipeline<sas::VulkanDynamicShader>;