#pragma once
#include <concepts>
#include <string_view>

namespace sas
{

    template <typename T>
    concept Inspectable = requires(T &component) {
        {
            component.onInspect()
        } -> std::same_as<void>;
    };

    struct InspectableComponent
    {
        std::string_view name;
        void *componentPtr;
        void (*renderUI)(void *ptr);

        void draw() const noexcept
        {
            if (renderUI && componentPtr)
            {
                renderUI(componentPtr);
            }
        }
    };

};