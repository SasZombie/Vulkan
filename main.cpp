// Phase 1
#include "vkInitialize.hpp"
#include "vkPhysicalDevices.hpp"
#include "vkDevice.hpp"
#include "Window.hpp"
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

#include "Mesh.hpp"

#include "CrimsonBlossom.hpp"
#include "AssetManager.hpp"
#include "Logger.hpp"

extern "C" const char *__lsan_default_suppressions();

using namespace sas;

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
        BaseLogger::addLogger("Asset");
        BaseLogger::addLogger("Material");
        Window window(800, 600, "Meow");
        Camera camera{math::Vec3{0, 0, 3.f}};

        sas::CrimsonBlossom engine{window, camera};

        Scene firstScene;

        uint32_t sphereEntity = firstScene.sceneRegistry.createEntity();
        uint32_t cubeEntity = firstScene.sceneRegistry.createEntity();

        RenderMesh spereMesh = engine.assetManager.loadMesh("resources/models/sphere.obj");
        RenderMesh cubeMesh = engine.assetManager.loadMesh("resources/models/Cube.obj");

        RenderTexture renderTexture = engine.assetManager.materialManager.loadTexture("resources/textures/goldTexture.bmp");
        VulkanDynamicShader &shader = engine.assetManager.materialManager.loadShader("shaders/spv/vert.spv", "shaders/spv/frag.spv");

        Material* firstMaterial = engine.assetManager.materialManager.addMaterial("Golden Material", shader, renderTexture);

        RenderObject ShpereObj;
        RenderObject CubeObj;

        ShpereObj.mesh = &spereMesh;
        CubeObj.mesh = &cubeMesh;

        ShpereObj.material = firstMaterial;
        CubeObj.material = firstMaterial;

        firstScene.sceneRegistry.addComponent(sphereEntity, ShpereObj);
        firstScene.sceneRegistry.addComponent(cubeEntity, CubeObj);

        engine.addScene(firstScene);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            window.processKeyboardInput(camera);
            engine.update();
        }
    }

    glfwTerminate();
    BaseLogger::log("==============================\nFinished app\n==============================");
}

#if defined(__has_feature)
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
extern "C" const char *__lsan_default_suppressions()
{
    return "leak:libdbus-1.so\n"
           "leak:libnvidia-glcore.so\n"
           "leak:libGLX_nvidia.so\n"
           "leak:libnvidia-allocator.so\n"
           "leak:libnvidia-glsi.so\n"
           "leak:libwindow-decorations-gtk-module.so\n"
           "leak:libcolorreload-gtk-module.so\n"
           "leak:libdecor-gtk.so\n"
           "leak:libgtk-3.so\n"
           "leak:<unknown module>\n";
}
#endif
#endif
