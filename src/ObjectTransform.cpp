#include "ObjectTransform.hpp"
#include "imgui.h"

void sas::ObjectTransform3D::onInspect()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat3("Position", &position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
        ImGui::DragFloat3("Scale", &scale.x, 0.1f);
    }
}