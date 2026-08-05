#include "window.hpp"
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
}

#include <iostream>

void sas::Window::processInput(Camera& camera) noexcept
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        std::cout << "Move front\n";

        camera.move({0, 0, -1});

    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        std::cout << "Move back\n";

        camera.move({0, 0, 1});
    }
}
