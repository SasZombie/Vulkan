#include "vkShaderPipeline.hpp"
#include "vkComponents.hpp"

#include <vector>
#include <fstream>

static std::vector<uint32_t> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    const size_t fileSize = static_cast<size_t>(file.tellg());

    if (fileSize % sizeof(uint32_t) != 0)
    {
        throw std::runtime_error("SPIR-V file size is not a multiple of 4: " + filename);
    }

    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);

    file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

void sas::VulkanDynamicShader::populateShader(VkShaderEXT &shader, const VulkanShaderConfig &config, const std::vector<uint32_t> &codeData) noexcept
{
    const auto &descLay = config.descriptor;

    VkShaderCreateInfoEXT shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    shaderStageInfo.stage = config.stage;
    shaderStageInfo.nextStage = config.nextStage;
    shaderStageInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    shaderStageInfo.codeSize = codeData.size() * sizeof(uint32_t);
    shaderStageInfo.pCode = codeData.data();
    shaderStageInfo.pName = config.entryPoint;
    shaderStageInfo.setLayoutCount = 1;
    shaderStageInfo.pSetLayouts = &descLay;

    // ✅ Pass the exact same layout signature to ALL shaders.
    // Even if this is the Fragment shader, it needs to know that the 
    // overall pipeline layout contains a Vertex push constant.
    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Always VERTEX_BIT, matching your pipeline layout
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstants);

    shaderStageInfo.pushConstantRangeCount = 1;
    shaderStageInfo.pPushConstantRanges = &pushRange;

    vkCreateShadersEXT(device, 1, &shaderStageInfo, nullptr, &shader);
}

sas::VulkanDynamicShader::VulkanDynamicShader(VulkanDevice &dev, const VulkanDescriptor &desc)
    : device(dev)
{
    const auto &vertCode = readFile("shaders/spv/vert.spv");
    const auto &fragCode = readFile("shaders/spv/frag.spv");

    if (vertCode.empty() || fragCode.empty())
    {
        throw std::runtime_error("Cannot open vert or frag code");
    }

    const auto &descLay = desc.getDescriptorLayout();
    VulkanShaderConfig vertexConf{
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .descriptor = descLay,
        .nextStage = VK_SHADER_STAGE_FRAGMENT_BIT};

    VulkanShaderConfig fragConf{
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .descriptor = descLay};

    populateShader(vertexShader, vertexConf, vertCode);
    populateShader(fragmentShader, fragConf, fragCode);
}
