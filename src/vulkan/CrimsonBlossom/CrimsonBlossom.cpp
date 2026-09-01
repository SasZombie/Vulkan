#include "CrimsonBlossom.hpp"
#include "Transform.hpp"

std::vector<sas::DrawingComponents> sas::CrimsonBlossom::getDrawingData() noexcept
{
    auto it = scenes.find(activeSceneId);
    if (it == scenes.end())
    {
        return {};
    }
    auto renderData = it->second.sceneRegistry.getCombined<RenderObject, ObjectTransform3D>();

    std::sort(renderData.begin(), renderData.end(), [](const Combined<RenderObject, ObjectTransform3D> &rendObj, const Combined<RenderObject, ObjectTransform3D> &other)
              { return rendObj.get<RenderObject>()->material->shader->getId() < other.get<RenderObject>()->material->shader->getId(); });

    return renderData;
}

void sas::CrimsonBlossom::update() noexcept
{
    commandBus.processQueue();
    engineUi.updateFrame();

    vkRenderer.drawFrame(getDrawingData());
}

void sas::CrimsonBlossom::createUi() const noexcept
{
    
}