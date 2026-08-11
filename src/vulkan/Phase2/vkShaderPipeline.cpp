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
template <typename ShaderType>
sas::GenericVulkanPipeline<ShaderType>::GenericVulkanPipeline(VulkanDevice &vulkanDevice, VulkanDescriptor &desc) noexcept
    : device(vulkanDevice), descriptor(desc)
{
    std::cout << "Shader Constrsct clled \n";

    createShaders();
}

template <typename ShaderType>
void sas::GenericVulkanPipeline<ShaderType>::createShaders() noexcept
{
    auto vertShaderCode = readFile("shaders/spv/vert.spv");
    auto fragShaderCode = readFile("shaders/spv/frag.spv");

    if constexpr (std::is_same_v<ShaderType, VulkanShader>)
    {
        VulkanShader vert{}, frag{};

        auto populateShader = [&](VulkanShader &shader, std::vector<char> &data)
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
        };

        vert.shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        populateShader(vert, vertShaderCode);

        frag.shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        populateShader(frag, fragShaderCode);

        vertShader = ManagedShader<VulkanShader>(device, vert);
        fragShader = ManagedShader<VulkanShader>(device, frag);
    }
    else if constexpr (std::is_same_v<ShaderType, VulkanDynamicShader>)
    {
        VulkanDynamicShader vert{}, frag{};

        VkPushConstantRange pushRange{};
        pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushRange.offset = 0;
        pushRange.size = sizeof(PushConstants);

        const auto &descLay = descriptor.getDescriptorLayout();

        // VERTEX
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
        vertCreateInfo.setLayoutCount = 1;
        vertCreateInfo.pSetLayouts = &descLay;

        vkCreateShadersEXT(device, 1, &vertCreateInfo, nullptr, &vert.shaderModule);

        // FRAGMENT
        VkShaderCreateInfoEXT fragCreateInfo{};
        fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
        fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragCreateInfo.nextStage = 0;
        fragCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
        fragCreateInfo.codeSize = fragShaderCode.size();
        fragCreateInfo.pCode = reinterpret_cast<const uint32_t *>(fragShaderCode.data());
        fragCreateInfo.pName = "main";
        fragCreateInfo.pushConstantRangeCount = 1;
        fragCreateInfo.pPushConstantRanges = &pushRange;
        fragCreateInfo.setLayoutCount = 1;

        fragCreateInfo.pSetLayouts = &descLay;

        vkCreateShadersEXT(device, 1, &fragCreateInfo, nullptr, &frag.shaderModule);

        vertShader = ManagedShader<VulkanDynamicShader>(device, vert);
        fragShader = ManagedShader<VulkanDynamicShader>(device, frag);
    }
}

// template class sas::ManagedShader<sas::VulkanShader>;
// template class sas::ManagedShader<sas::VulkanDynamicShader>;

template class sas::GenericVulkanPipeline<sas::VulkanShader>;
template class sas::GenericVulkanPipeline<sas::VulkanDynamicShader>;