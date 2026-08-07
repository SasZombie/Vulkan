#pragma once

#include <memory>
#include <vector>
#include "Math.hpp"

namespace sas
{    
    struct SceneNode;
    using SceneSharedNode = std::shared_ptr<SceneNode>; 
    
    struct SceneNode : public std::enable_shared_from_this<SceneNode>
    {
        std::weak_ptr<SceneNode> parent;
        std::vector<SceneSharedNode> components;

        
        //Used only by a root node
        SceneNode() noexcept = default;
        SceneNode(const math::Vec3& position) noexcept;

        //99.9 children use this
        SceneNode(SceneSharedNode nparent) noexcept;
        
        void addNode(SceneSharedNode child) noexcept;

        virtual ~SceneNode() = default;
    };   

}