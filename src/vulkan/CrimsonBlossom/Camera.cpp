#include "Camera.hpp"

void sas::Camera::move(const math::Vec3 &movement) noexcept
{
    const float speed = 0.5f;
    position = position + (viewDirection * movement.z * speed);
    position = position + (rightVector * movement.x * speed);
    position = position + (worldUp * movement.y * speed);
}

void sas::Camera::setViewDirection(const math::Vec3 &viewDir) noexcept
{
    viewDirection = viewDir.normalized();

    rightVector = (viewDirection * worldUp).normalized();
    upVector = (rightVector * viewDirection).normalized();
}

sas::math::Mat4 sas::Camera::getViewProjection() const noexcept
{
    const math::Mat4 view = math::lookAt(position, position + viewDirection, upVector);
    const math::Mat4 proj = getProjection();

    math::Mat4 projView = proj * view;
    projView(1, 1) *= -1.f;
    return projView;
}

sas::math::Mat4 sas::Camera::getProjection() const noexcept
{
    return math::perspective(math::degToRad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}