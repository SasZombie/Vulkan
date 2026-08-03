cmake_minimum_required(VERSION 3.12)
project(Vulkan)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(project_warnings INTERFACE)
add_library(project_sanitizers INTERFACE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(project_warnings INTERFACE
        -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith
        -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow
        -Wwrite-strings -Wno-unused-parameter -Wfloat-equal
    )
    target_compile_options(project_sanitizers INTERFACE
        -fsanitize=undefined -fsanitize=address -fsanitize=leak -g
    )
    target_link_options(project_sanitizers INTERFACE
        -fsanitize=undefined -fsanitize=address -fsanitize=leak
    )
else()
    target_compile_options(project_warnings INTERFACE -O3)
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

SET(GLOBAL_SOURCES
    src/window.cpp
)

SET(PHASE_1_SOURCES
    vkInitialize.cpp
    vkPhysicalDevices.cpp
    vkDevice.cpp
    vkBridge.cpp
    vkCommand.cpp
)

list(TRANSFORM PHASE_1_SOURCES PREPEND "src/vulkan/Phase1/")

add_executable(main main.cpp ${GLOBAL_SOURCES} ${PHASE_1_SOURCES})

target_include_directories(main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/Phase1
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan/Phase2
)

target_link_libraries(main PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm

    project_warnings
    project_sanitizers
)
