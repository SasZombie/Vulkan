cmake_minimum_required(VERSION 3.12)
project(CrimsonBlossom)

add_compile_definitions(VK_NO_PROTOTYPES)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(project_warnings_vulkan INTERFACE)
add_library(project_sanitizers_vulkan INTERFACE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(project_warnings_vulkan INTERFACE
        -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith
        -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow
        -Wwrite-strings -Wno-unused-parameter -Wfloat-equal
    )
    target_compile_options(project_sanitizers_vulkan INTERFACE
        -fsanitize=undefined -fsanitize=address -fsanitize=leak -g
    )
    target_link_options(project_sanitizers_vulkan INTERFACE
        -fsanitize=undefined -fsanitize=address -fsanitize=leak
    )
else()
    target_compile_options(project_warnings INTERFACE -O3)
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_library(volk STATIC deps/volk.c)

target_include_directories(volk PUBLIC deps)
target_link_libraries(volk PUBLIC Vulkan::Headers ${CMAKE_DL_LIBS})

SET(ENGINE_SOURCES
    Camera.cpp
    SceneNode.cpp
    CrimsonBlossom.cpp
    AssetManager.cpp
    Material.cpp
    EngineUi.cpp
)

SET(RENDERER_SOURCES
    vkRenderer.cpp
)

SET(GLOBAL_SOURCES
    Window.cpp
    Logger.cpp
)

SET(VULKAN_SOURCES
    vkDescriptor.cpp
    vkSampler.cpp
    vkAllocator.cpp
)

SET(PHASE_1_SOURCES
    vkInitialize.cpp
    vkPhysicalDevices.cpp
    vkDevice.cpp
    vkBridge.cpp
    vkCommand.cpp
)

SET(PHASE_2_SOURCES
    vkInputPipeline.cpp
    vkShaderPipeline.cpp
    vkViewport.cpp
    vkPixelStyling.cpp
    vkRastarization.cpp
    vkMasterPipeline.cpp
)

list(TRANSFORM GLOBAL_SOURCES PREPEND "src/")
list(TRANSFORM PHASE_1_SOURCES PREPEND "src/vulkan/Phase1/")
list(TRANSFORM PHASE_2_SOURCES PREPEND "src/vulkan/Phase2/")
list(TRANSFORM ENGINE_SOURCES PREPEND "src/vulkan/CrimsonBlossom/")
list(TRANSFORM VULKAN_SOURCES PREPEND "src/vulkan/")
list(TRANSFORM RENDERER_SOURCES PREPEND "src/vulkan/Renderer/")


include(FetchContent)
FetchContent_Declare(
    dearImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8
    GIT_SHALLOW TRUE
    )


FetchContent_Declare(
    physicsEngine
    GIT_REPOSITORY https://github.com/SasZombie/Engine.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    )

FetchContent_MakeAvailable(dearImGui physicsEngine)

add_library(dearImGui STATIC
    ${dearimgui_SOURCE_DIR}/imgui.cpp
    ${dearimgui_SOURCE_DIR}/imgui_demo.cpp
    ${dearimgui_SOURCE_DIR}/imgui_draw.cpp
    ${dearimgui_SOURCE_DIR}/imgui_tables.cpp
    ${dearimgui_SOURCE_DIR}/imgui_widgets.cpp
    ${dearimgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${dearimgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(dearImGui PUBLIC
    ${dearimgui_SOURCE_DIR}
    ${dearimgui_SOURCE_DIR}/backends
)

add_executable(main main.cpp ${GLOBAL_SOURCES} ${PHASE_1_SOURCES} ${PHASE_2_SOURCES} ${VULKAN_SOURCES} ${ENGINE_SOURCES} ${RENDERER_SOURCES})

target_include_directories(main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CrimsonBlossom
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/Phase1
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/Phase2
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/
)

target_link_libraries(main PRIVATE
    volk
    glfw
    sas_physics
    project_warnings_vulkan
    project_sanitizers_vulkan
    dearImGui
)
