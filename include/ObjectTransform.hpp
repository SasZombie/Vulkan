#pragma once
#include "Math.hpp"

namespace sas
{

    struct ObjectTransform2D
    {
        float rotation = 0;
        math::Vec2 position;
        math::Vec2 scale{1.f, 1.f};

        ObjectTransform2D() noexcept = default;
        explicit ObjectTransform2D(const math::Vec2 &pos) noexcept
            : position(pos)
        {
        }

        math::Mat4 getModelMatrix() const noexcept
        {
            //TODO
            
            return {};
        }
    };

    struct ObjectTransform3D
    {
        math::Vec3 rotation;
        math::Vec3 position;
        math::Vec3 scale{1.f, 1.f, 1.f};

        ObjectTransform3D() noexcept = default;
        explicit ObjectTransform3D(const math::Vec3 &pos) noexcept
            : position(pos)
        {
        }

        math::Mat4 getModelMatrix() const noexcept
        {
            math::Mat4 model;

            model = math::translate(model, position);

            model = math::rotate(model, rotation.x, {1.0f, 0.0f, 0.0f});
            model = math::rotate(model, rotation.y, {0.0f, 1.0f, 0.0f});
            model = math::rotate(model, rotation.z, {0.0f, 0.0f, 1.0f});

            model = math::scale(model, scale);

            return model;
        }
    };

}