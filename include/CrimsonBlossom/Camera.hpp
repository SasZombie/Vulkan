#pragma once

#include "Math.hpp"

namespace sas
{

    class Camera
    {
    private:
        math::Vec3 position;
        math::Vec3 viewDirection;
        math::Vec3 upVector;
        math::Vec3 rightVector;

    public:
        Camera(const math::Vec3 &cameraPosition) noexcept
            : position(cameraPosition), viewDirection(0.f, 0.f, -1.f),
              upVector(0.f, 1.f, 0.f), rightVector(viewDirection * upVector)
        {
        }

        Camera(const math::Vec3 &cameraPosition, const math::Vec3 &cameraViewDirection, const math::Vec3 &cameraUp) noexcept
            : position(cameraPosition), viewDirection(cameraViewDirection), upVector(cameraUp), rightVector(viewDirection * upVector) 
        {
        }

        void move(const math::Vec3& movement) noexcept;

        math::Mat4 getViewProjection() const noexcept;
        math::Mat4 getProjection() const noexcept;

        void setViewDirection(const math::Vec3& viewDir) noexcept;
        [[nodiscard]]math::Mat4 getMVP() const noexcept;
    };

} // namespace sas
