#include "vkShaderPipeline.hpp"

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

void VulkanShaderPipeline::populateShader(VulkanShader &shader, std::vector<char> &data)
{
    VkShaderModuleCreateInfo createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = data.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(data.data());

    vkCreateShaderModule(device, &createInfo, nullptr, &shader.shaderModule);

    // HERE CAN BE MORE STEPS IF NEEDED

    shader.shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader.shaderStageInfo.module = shader.shaderModule;
    shader.shaderStageInfo.pName = "main";
}

VulkanShaderPipeline::VulkanShaderPipeline(VulkanDevice &vulkanDevice)
    : device(vulkanDevice)
{
    vertShader.shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    auto vertShaderCode = readFile("shaders/spv/vert.spv");
    populateShader(vertShader, vertShaderCode);

    fragShader.shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    auto fragShaderCode = readFile("shaders/spv/frag.spv");
    populateShader(fragShader, fragShaderCode);
}