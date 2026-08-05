#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

#include "Camera.hpp"

namespace sas
{

    class Window
    {
    private:
        math::Vec2 size;

    public:
        GLFWwindow *window;

        Window(int width, int height, const std::string &title);

        void processInput(Camera& camera) noexcept;

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

        size_t getWidth() const noexcept
        {
            return size.x;
        }

        size_t getHeight() const noexcept
        {
            return size.y;
        }

        std::pair<size_t, size_t> getSize() const noexcept
        {
            return std::make_pair(size.x, size.y);
        }

        operator GLFWwindow *() noexcept
        {
            return window;
        }
    };

} // namespace sas
