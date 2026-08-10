#include "CrimsonBlossom.hpp"


sas::RenderObject sas::CrimsonBlossom::createBuffer(const MeshComponent& component) const noexcept
{


}

void sas::CrimsonBlossom::update() noexcept
{

    const auto& allMeshComp = scenes[0].sceneRegistry.getComponents<RenderObject>();
    
    vkRenderer.drawFrame(allMeshComp->getData());
    
}