#include "RenderPass.hpp"
#include "RenderObject.hpp"
#include "ObjectTransform.hpp"

void sas::MainScenePass::record(const std::vector<DrawingComponents> &objectsToRender, const RenderPassComponents& components) const noexcept
{
    uint32_t prevId = 0;

    for (const auto &combinedItem : objectsToRender)
    {
        const auto &renderObj = *combinedItem.get<RenderObject>();

        if (renderObj.material->shader->getId() != prevId)
        {
            VkShaderStageFlagBits stages[] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
            const auto &[vert, frag] = renderObj.material->shader->getShaderModule();

            VkShaderEXT shaders[] = {vert, frag};

            vkCmdBindShadersEXT(components.devices.vkCommand.getCommandBuffer(), 2, stages, shaders);

            prevId = renderObj.material->shader->getId();
        }

        drawCallRecorder(combinedItem, components);
    }
}

void sas::MainScenePass::drawCallRecorder(const DrawingComponents &component, const RenderPassComponents& components) const noexcept
{
    const auto &renderObj = *component.get<RenderObject>();

    const auto &comandBuff = components.devices.vkCommand.getCommandBuffer();
    const auto &descriptor = renderObj.material->descriptorSet;

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(comandBuff, 0, 1, &renderObj.mesh->vertexBuffer, offsets);
    vkCmdBindIndexBuffer(comandBuff, renderObj.mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
        comandBuff,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        components.masterPipe.getGraphicsPipelineLayout(),
        0,
        1,
        &descriptor,
        0, nullptr);

    const auto &transform = *component.get<ObjectTransform3D>();
    const auto &modelMat = transform.getModelMatrix();

    const auto &finalMvp = components.camera.getViewProjection() * modelMat;

    PushConstants constants{finalMvp};

    vkCmdPushConstants(comandBuff,
                       components.masterPipe.getGraphicsPipelineLayout(),
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(PushConstants),
                       &constants);

    vkCmdDrawIndexed(comandBuff, renderObj.mesh->indexCount, 1, 0, 0, 0);
}
