#include "CrimsonBlossom.hpp"
#include "Transform.hpp"


void sas::CrimsonBlossom::update() noexcept
{

    if(activeSceneId == std::numeric_limits<uint32_t>::max())
    {
        return;
    }
    auto renderData = scenes[activeSceneId].sceneRegistry.getCombined<RenderObject, ObjectTransform3D>();
    
    std::sort(renderData.begin(), renderData.end(), [](const Combined<RenderObject, ObjectTransform3D> & rendObj, const Combined<RenderObject, ObjectTransform3D> & other){
        return rendObj.get<RenderObject>()->material->shader->getId() < other.get<RenderObject>()->material->shader->getId();
    });
    
    engineUi.updateFrame();

    vkRenderer.drawFrame(renderData);
    
}

void sas::CrimsonBlossom::createUi() const noexcept
{
    
}