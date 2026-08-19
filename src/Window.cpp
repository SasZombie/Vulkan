#include "Window.hpp"
#include <stdexcept>

sas::Window::Window(int nwidth, int nheight, const std::string &title)
    : size(nwidth, nheight)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(nwidth, nheight, title.c_str(), nullptr, nullptr);

    if (window == nullptr)
    {
        throw std::runtime_error("Failed to create GFLW window\n");
    }

    // glfwSetWindowUserPointer(window, this);
    // glfwSetCursorPosCallback(window, mouse_callback);
}

float yaw = -90.f, pitch = 0.f;

double lastX = 400, lastY = 300;

//TODO: Make this work normally
void sas::Window::processKeyboardInput(Camera &camera) noexcept
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.move({0, 0, -1});
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.move({0, 0, 1});
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.move({1, 0, 0});
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.move({-1, 0, 0});
    }

    float rotationSpeed = 0.4f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        yaw -= rotationSpeed; // Subtracting yaw turns the camera left

        // Recalculate and update the camera's view direction
        sas::math::Vec3 front;
        front.x = cos(sas::math::degToRad(yaw)) * cos(sas::math::degToRad(pitch));
        front.y = sin(sas::math::degToRad(pitch));
        front.z = sin(sas::math::degToRad(yaw)) * cos(sas::math::degToRad(pitch));

        camera.setViewDirection(front.normalized());
    }

    // --- Rotate Right with Right Arrow or 'E' ---
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        yaw += rotationSpeed;

        sas::math::Vec3 front;
        front.x = cos(sas::math::degToRad(yaw)) * cos(sas::math::degToRad(pitch));
        front.y = sin(sas::math::degToRad(pitch));
        front.z = sin(sas::math::degToRad(yaw)) * cos(sas::math::degToRad(pitch));

        camera.setViewDirection(front.normalized());
    }
}


void mouse_callback(GLFWwindow *glWindow, double xpos, double ypos) noexcept
{

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.15f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.f)
        pitch = 89.f;
    if (pitch < -89.f)
        pitch = -89.f;

    if (yaw > 360)
        yaw = yaw - 360;
    if (yaw < 0)
        yaw = yaw + 360;
}

void sas::Window::processMouseInput(Camera &camera) noexcept
{
    sas::math::Vec3 front;
    front.x = cos(sas::math::degToRad(yaw)) * cos(sas::math::degToRad(pitch));
    front.y = sin(sas::math::degToRad(pitch));
    front.z = sin(sas::math::degToRad(yaw)) * cos(sas::math::degToRad(pitch));
    camera.setViewDirection(front.normalized());
}
