#include "CrimsonBlossom.hpp"

void sas::CrimsonBlossom::update() noexcept
{

    auto& renderData = scenes[0].sceneRegistry.getComponents<RenderObject>()->getData();
    
    std::sort(renderData.begin(), renderData.end(), [](const RenderObject& rendObj, const RenderObject& other){
        return rendObj.material->shader->getId() < other.material->shader->getId();
    });

    vkRenderer.drawFrame(renderData);
    
}