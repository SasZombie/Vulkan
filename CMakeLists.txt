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
    Mesh.cpp
)

SET(GLOBAL_SOURCES
    Window.cpp
)

SET(VULKAN_SOURCES
    vkDescriptor.cpp
    vkSampler.cpp
    vkRenderer.cpp
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

list(TRANSFORM GLOBAL_SOURCES  PREPEND "src/")
list(TRANSFORM PHASE_1_SOURCES PREPEND "src/vulkan/Phase1/")
list(TRANSFORM PHASE_2_SOURCES PREPEND "src/vulkan/Phase2/")
list(TRANSFORM ENGINE_SOURCES  PREPEND "src/vulkan/CrimsonBlossom/")
list(TRANSFORM VULKAN_SOURCES  PREPEND "src/vulkan/")

add_executable(main main.cpp ${GLOBAL_SOURCES} ${PHASE_1_SOURCES} ${PHASE_2_SOURCES} ${VULKAN_SOURCES} ${ENGINE_SOURCES})
add_subdirectory(deps/Engine)


target_include_directories(main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CrimsonBlossom
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/Phase1
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/Phase2
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/Engine/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/
    
)

target_link_libraries(main PRIVATE
    volk
    glfw
    sas_physics
    project_warnings_vulkan
    project_sanitizers_vulkan
)