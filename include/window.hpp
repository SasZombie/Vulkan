#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
    GLFWwindow *window;

    Window(size_t width, size_t height, const std::string &title);

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    Window(Window &&other) noexcept : window(other.window)
    {
        other.window = nullptr;
    }

    Window &operator=(Window &&other) noexcept
    {
        if (this != &other)
        {
            if (window != nullptr)
            {
                glfwDestroyWindow(window);
            }
            window = other.window;
            other.window = nullptr;
        }
        return *this;
    }

    ~Window() noexcept
    {
        if (window != nullptr)
        {
            glfwDestroyWindow(window);
        }
    }
};