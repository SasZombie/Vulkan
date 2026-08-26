#include "Camera.hpp"

void sas::Camera::move(const math::Vec3 &movement) noexcept
{
    position = position + movement;
}

void sas::Camera::setViewDirection(const math::Vec3 &viewDir) noexcept
{
    viewDirection = viewDir;
    rightVector = viewDir * upVector;
}

sas::math::Mat4 sas::Camera::getViewProjection() const noexcept
{
    math::Mat4 view = math::lookAt(position, position + viewDirection, upVector);
    math::Mat4 proj = math::perspective(math::degToRad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    return proj * view;
}

sas::math::Mat4 sas::Camera::getProjection() const noexcept
{
    return math::perspective(math::degToRad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

sas::math::Mat4 sas::Camera::getMVP() const noexcept
{
    math::Mat4 model;

    math::Mat4 view = math::lookAt(position, position + viewDirection, upVector);

    math::Mat4 proj = math::perspective(math::degToRad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    return proj * view * model;
}
