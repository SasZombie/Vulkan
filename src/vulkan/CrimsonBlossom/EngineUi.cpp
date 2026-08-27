#include "EngineUi.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <stdexcept>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <array>

sas::EngineUi::EngineUi(VulkanDevices &dev, const Window &window) noexcept
    : device(dev.vkDevice)
{
    initPool();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_LoadFunctions([](const char *function_name, void *vulkan_instance)
                                   { return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance *>(vulkan_instance)), function_name); }, &dev.vk);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = dev.vk;
    initInfo.PhysicalDevice = dev.vkPhysical;
    initInfo.Device = device;
    initInfo.QueueFamily = dev.vkDevice.getGraphicsQueueFamilyIndex();
    initInfo.Queue = dev.vkDevice.getQueue();
    initInfo.DescriptorPool = uiPool;
    // TODO: This is hardcoded in both parts
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = 3;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    initInfo.UseDynamicRendering = true;
    const auto &surfaceFormat = dev.vkBridge.getSurfaceFormat();

    VkPipelineRenderingCreateInfoKHR createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    createInfo.colorAttachmentCount = 1;
    createInfo.pColorAttachmentFormats = &surfaceFormat.format;

    initInfo.PipelineRenderingCreateInfo = createInfo;

    ImGui_ImplVulkan_Init(&initInfo);
}

sas::EngineUi::~EngineUi() noexcept
{
    vkDeviceWaitIdle(device);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(device, uiPool, nullptr);
}

void sas::EngineUi::initPool()
{
    VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 100},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100}};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000;
    poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    poolInfo.pPoolSizes = poolSizes;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &uiPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create ImGui descriptor pool!");
    }
}

void sas::EngineUi::newFrame() const noexcept
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void sas::EngineUi::writeFrames() const noexcept
{
    ImGui::Begin("Inspector");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void sas::EngineUi::updateFrame() const noexcept
{
    newFrame();
    writeFrames();
}