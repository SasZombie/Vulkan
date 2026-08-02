#include "vkInitialize.hpp"
#include "vkPhysicalDevices.hpp"
#include "vkDevice.hpp"
#include "window.hpp"

extern "C" const char *__lsan_default_suppressions();

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

int main()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW!\n");
    }

    if (!glfwVulkanSupported())
    {
        glfwTerminate();
        throw std::runtime_error("Vulkan is NOT supported on this system!");
    }

    Window window(800, 600, "Meow");
    VulkanInstanceWrapper vk;
    VulkanPhysicalDevice vkPhysical(vk);
    VulkanDevice vkDevice(vkPhysical);
    

    glfwTerminate();
    std::cout << "\n==============================\nFinished app\n==============================\n";
}
