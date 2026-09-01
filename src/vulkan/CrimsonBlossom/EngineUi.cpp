#include "EngineUi.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "EntityRegistry.hpp"
#include "ObjectTransform.hpp"
#include "RenderObject.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <stdexcept>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <array>

sas::EngineUi::EngineUi(VulkanDevices &dev, const Window &window, CommandBus &bus) noexcept
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

    pannels.push_back(std::make_unique<EditorPannel>(bus));
    pannels.push_back(std::make_unique<FPSpannel>(bus));
    pannels.push_back(std::make_unique<ObjectInspector>(bus));
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
    for (const auto &panel : pannels)
    {
        panel->renderUI();
    }
}

void sas::EngineUi::updateFrame() const noexcept
{
    newFrame();
    writeFrames();
}

void sas::EditorPannel::renderUI() const noexcept
{
    ImGui::Begin("EditorPannel");
    ImGui::Text("Meshes");

    // Mesh* selectedMesh = nullptr;
    // Material* selectedMaterial = nullptr;

    QuerryMapCommand<std::string, Mesh> meshes;
    commandBus.dispatch(meshes);

    if (meshes.map)
    {
        for (const auto &[name, mesh] : *meshes.map)
        {
            ImGui::PushID(name.c_str());

            if (ImGui::TreeNode(name.c_str()))
            {
                ImGui::Text("Vertices: %zu", mesh.vertices.size());
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    static char text[50] = "";

    bool isNotNull = (text[0] != '\0');

    ImGui::BeginDisabled(!isNotNull);
    if (ImGui::Button("Add Mesh"))
    {
        const std::string path = "resources/models/";
        commandBus.post<CreateNewMeshCommand>(CreateNewMeshCommand{path + text});

        text[0] = '\0';
    }
    ImGui::EndDisabled();
    ImGui::InputText("##textarea", text, sizeof(text));

    ImGui::Separator();
    ImGui::Text("Materials");

    QuerryMapCommand<std::string, Material> materials;
    commandBus.dispatch(materials);

    if (materials.map)
    {
        for (const auto &[name, material] : *materials.map)
        {
            ImGui::PushID(name.c_str());

            if (ImGui::TreeNode(name.c_str()))
            {
                ImGui::Text("Shader Id: %u", material.shader->getId());
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }
    ImGui::Separator();

    ImGui::Text("Rendered Objects");

    // TODO: this grabs more than what I need
    QuerryListCommand<Combined<RenderObject, ObjectTransform3D>> renderObjects;
    commandBus.dispatch(renderObjects);

    if (!renderObjects.objList.empty())
    {
        for (const auto &combinedObject : renderObjects.objList)
        {
            ImGui::Text(std::to_string(combinedObject.entity).c_str());

            if (ImGui::IsItemClicked())
            {
                commandBus.post<ItemSelected>(ItemSelected{combinedObject.entity});
            }
        }
    }

    ImGui::Separator();

    static char meshBuff[50];
    static char materialBuff[50];

    bool isMeshBufferNotNull = (meshBuff[0] != '\0');
    bool isMaterialBufferNotNull = (materialBuff[0] != '\0');

    ImGui::InputText("##meshTextArea", meshBuff, sizeof(meshBuff));
    ImGui::InputText("##materialTextArea", materialBuff, sizeof(materialBuff));

    ImGui::BeginDisabled(!(isMeshBufferNotNull && isMaterialBufferNotNull));
    if (ImGui::Button("Add Object"))
    {
        commandBus.post<CreateNewEntityCommand>(CreateNewEntityCommand{meshBuff, materialBuff});

        meshBuff[0] = '\0';
        materialBuff[0] = '\0';
    }
    ImGui::EndDisabled();

    ImGui::End();
}

void sas::FPSpannel::renderUI() const noexcept
{
    ImGui::Begin("FpsPannel");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void sas::ObjectInspector::renderUI() const noexcept
{
    ImGui::Begin("InspectorPannel");
    if (currentItemId == std::numeric_limits<uint32_t>::max())
    {
        ImGui::End();
        return;
    }

    QuerryEntityComponentsCommand components(currentItemId);
    commandBus.dispatch(components);

    for (const auto &component : components.components)
    {
        component.draw();
    }

    ImGui::End();
}
