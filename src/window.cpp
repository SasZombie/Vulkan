#include "window.hpp"
#include <stdexcept>

Window::Window(size_t width, size_t height, const std::string &title)
{
            window = glfwCreateWindow(800, 600, "Vulkan Arch Test", nullptr, nullptr);

        if (window == nullptr)
        {
            throw std::runtime_error("Failed to create GFLW window\n");
        }
    
}