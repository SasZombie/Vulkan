#include "CrimsonBlossom.hpp"
#include "Transform.hpp"


void sas::CrimsonBlossom::update() noexcept
{

    // auto& renderData = scenes[0].sceneRegistry.getComponents<RenderObject>()->getData();

    auto renderData = scenes[0].sceneRegistry.getCombined<RenderObject, ObjectTransform3D>();
    
    std::sort(renderData.begin(), renderData.end(), [](const Combined<RenderObject, ObjectTransform3D> & rendObj, const Combined<RenderObject, ObjectTransform3D> & other){
        return rendObj.get<RenderObject>()->material->shader->getId() < other.get<RenderObject>()->material->shader->getId();
    });

    vkRenderer.drawFrame(renderData);
    
}