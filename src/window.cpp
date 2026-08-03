#include "window.hpp"
#include <stdexcept>

Window::Window(size_t nwidth, size_t nheight, const std::string &title)
    : width(nwidth), height(nheight)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Vulkan Arch Test", nullptr, nullptr);

    if (window == nullptr)
    {
        throw std::runtime_error("Failed to create GFLW window\n");
    }
}