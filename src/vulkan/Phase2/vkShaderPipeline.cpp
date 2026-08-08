#include "vkShaderPipeline.hpp"

#include "vkComponents.hpp"
#include <vector>
#include <fstream>

static std::vector<char> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    size_t fileSize = static_cast<size_t>(file.tellg());

    if (fileSize % 4 != 0)
    {
        throw std::runtime_error("SPIR-V file size is not a multiple of 4: " + filename);
    }

    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

// void sas::VulkanShaderPipeline::populateShader(VulkanShader &shader, std::vector<char> &data)
// {
//     VkShaderModuleCreateInfo createInfo;

//     createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//     createInfo.codeSize = data.size();
//     createInfo.pCode = reinterpret_cast<const uint32_t *>(data.data());

//     vkCreateShaderModule(device, &createInfo, nullptr, &shader.shaderModule);

//     // HERE CAN BE MORE STEPS IF NEEDED

//     shader.shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     shader.shaderStageInfo.module = shader.shaderModule;
//     shader.shaderStageInfo.pName = "main";
// }

sas::VulkanShaderPipeline::VulkanShaderPipeline(VulkanDevice &vulkanDevice)
    : device(vulkanDevice)
{

    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushRange.offset = 0;
    pushRange.size = sizeof(PushConstants);

    auto vertShaderCode = readFile("shaders/spv/vert.spv");

    // VERTEX SHADER
    VkShaderCreateInfoEXT vertCreateInfo{};
    vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertCreateInfo.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vertCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    vertCreateInfo.codeSize = vertShaderCode.size();
    vertCreateInfo.pCode = reinterpret_cast<const uint32_t *>(vertShaderCode.data());
    vertCreateInfo.pName = "main";
    vertCreateInfo.pushConstantRangeCount = 1;
    vertCreateInfo.pPushConstantRanges = &pushRange;

    vkCreateShadersEXT(device, 1, &vertCreateInfo, nullptr, &vertShader.shaderModule);

    // FRAGMENT SHADER
    auto fragShaderCode = readFile("shaders/spv/frag.spv");

    VkShaderCreateInfoEXT fragCreateInfo{};
    fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
    fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragCreateInfo.nextStage = 0;
    fragCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    fragCreateInfo.codeSize = fragShaderCode.size();
    fragCreateInfo.pCode = reinterpret_cast<const uint32_t *>(fragShaderCode.data());
    fragCreateInfo.pName = "main";

    vkCreateShadersEXT(device, 1, &fragCreateInfo, nullptr, &fragShader.shaderModule);
}