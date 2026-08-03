// Phase 1
#include "vkInitialize.hpp"
#include "vkPhysicalDevices.hpp"
#include "vkDevice.hpp"
#include "window.hpp"
#include "vkBridge.hpp"
#include "vkCommand.hpp"

// Phase 2
#include "vkShaderPipeline.hpp"
#include "vkInputPipeline.hpp"
#include "vkViewport.hpp"
#include "vkPixelStyling.hpp"
#include "vkRastarization.hpp"
// Final Pipe

#include "vkMasterPipeline.hpp"

extern "C" const char *__lsan_default_suppressions();

int main()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW!\n");
    }

    glfwSetErrorCallback([](int error, const char *description)
                         { std::cerr << "[GLFW Error " << error << "]: " << description << '\n'; });

    if (!glfwVulkanSupported())
    {
        glfwTerminate();
        throw std::runtime_error("Vulkan is NOT supported on this system!");
    }

    {
        Window window(800, 600, "Meow");

        VulkanInstanceWrapper vk;
        VulkanPhysicalDevice vkPhysical{vk};
        VulkanDevice vkDevice{vkPhysical};
        VulkanBridge vkBridge{window, vk, vkPhysical, vkDevice};

        VulkanCommand vkCommand{vkDevice};

        // Phase 2
        VulkanInputPipeline inputPipeline;
        VulkanShaderPipeline shaderPipeline{vkDevice};
        VulkanViewport viewPort{window};
        VulkanPixelStyling pixelStyle;
        VulkanRastarization raster;
        
        // Final Pipe

        VulkanPipelineComponents components{&vkBridge, &shaderPipeline, &inputPipeline, &viewPort, &raster, &pixelStyle};

        VulkanMasterPipeline masterPipeline{vkDevice, components};
    }

    glfwTerminate();
    std::cout << "\n==============================\nFinished app\n==============================\n";
}

#if defined(__has_feature)
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
extern "C" const char *__lsan_default_suppressions()
{
    return "leak:libdbus-1.so\n"
           "leak:libnvidia-glcore.so\n"
           "leak:libGLX_nvidia.so\n"
           "leak:libnvidia-allocator.so\n"
           "leak:<unknown module>\n"
           "leak:libnvidia-glsi.so\n";
}
#endif
#endif
