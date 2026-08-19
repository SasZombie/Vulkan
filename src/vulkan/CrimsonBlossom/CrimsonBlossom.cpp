#include "CrimsonBlossom.hpp"


// sas::RenderObject sas::CrimsonBlossom::createBuffer(const MeshComponent& component) const noexcept
// {


// }

void sas::CrimsonBlossom::update() noexcept
{

    auto& renderData = scenes[0].sceneRegistry.getComponents<RenderObject>()->getData();
    
    std::sort(renderData.begin(), renderData.end(), [](const RenderObject& rendObj, const RenderObject& other){
        return rendObj.shader->getId() < other.shader->getId();
    });

    vkRenderer.drawFrame(renderData);
    
}