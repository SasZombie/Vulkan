// #include "SceneNode.hpp"
// #include <fstream>

// sas::SceneNode::SceneNode(const glm::vec3 &position) noexcept
// {
//     worldTransform.position = position;
// }

// sas::SceneNode::SceneNode(SceneSharedNode nparent) noexcept
//     : parent(std::move(nparent))
// {
// }

// void sas::SceneNode::addNode(SceneSharedNode child) noexcept
// {
//     child->parent = shared_from_this();
//     components.push_back(std::move(child));
// }

// void sas::SceneNode::uppdateWorldTransformCamera(const Transform &parentWorldTransform) noexcept
// {
//     // std::cout << "Camera local transform = " << localTransform << "\n parent World transform = " << parentWorldTransform << '\n' ;
//     worldTransform = parentWorldTransform.combine(localTransform);

//     for (auto &child : components)
//         child->uppdateWorldTransformCamera(worldTransform);
// }

// void sas::SceneNode::uppdate(const Camera *camera) noexcept
// {

//     // velocity += acceleration * Globals::instance().getDeltaTime();
//     // localTransform.position += velocity * Globals::instance().getDeltaTime();
//     for (auto &component : components)
//     {
//         component->uppdate(camera);
//     }
// }

// void sas::SceneNode::uppdateAttachedToCamera(const Camera *camera) noexcept
// {
//     for (auto &component : components)
//     {
//         component->uppdateAttachedToCamera(camera);
//     }
// }

// glm::vec4 sas::SceneNode::getClipSpacePos() const noexcept
// {
//     return glm::vec4(1.f, 0.f, 0.f, 1.f);
// }

// void sas::SceneNode::save(std::ofstream &out) noexcept
// {
//     out << localTransform << worldTransform;

//     for (auto &child : components)
//     {
//         child->save(out);
//     }
// }

// void sas::SceneNode::uppdateWorldTransform(const Transform &parentWorldTransform) noexcept
// {
//     worldTransform = parentWorldTransform.combine(localTransform);
//     for (auto &child : components)
//         child->uppdateWorldTransform(worldTransform);
// }
