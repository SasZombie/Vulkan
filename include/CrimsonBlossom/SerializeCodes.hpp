#pragma once

#include <iostream>
#include <utility>
#include <meta>
#include <type_traits>
#include <string_view>

namespace sas
{

    enum struct SerializeCodesBinary
    {
        COMMENT,
        META_INFO,
        MAJOR_VERSION,
        MINOR_VERSION,
        PATCH_VERSION,
        START_SCENE,
        SCENE_FORMAT_VERSION,
        ID,
        NAME,
        NUMBER_OF_SUB_OBJECTS,
        END

    };

    enum struct SerializeCodesText
    {
        COMMENT,
        META_INFO,
        MAJOR_VERSION,
        MINOR_VERSION,
        PATCH_VERSION,
        START_SCENE,
        SCENE_FORMAT_VERSION,
        ID,
        NAME,
        NUMBER_OF_SUB_OBJECTS,
        END
    };

    std::ostream &operator<<(std::ostream &os, SerializeCodesText code);
    std::ostream &operator<<(std::ostream &os, SerializeCodesBinary code);


    template <typename E1, typename E2>
        requires(std::is_enum_v<E1> && std::is_enum_v<E2>)
    consteval bool are_enums_identical() noexcept
    {
        auto list1 = std::meta::enumerators_of(^^E1);
        auto list2 = std::meta::enumerators_of(^^E2);

        if (list1.size() != list2.size())
        {
            return false;
        }

        for (std::size_t i = 0; i < list1.size(); ++i)
        {
            if (std::meta::identifier_of(list1[i]) != std::meta::identifier_of(list2[i]))
            {
                return false;
            }
        }
        return true;
    }

    static_assert(are_enums_identical<SerializeCodesText, SerializeCodesBinary>(),
                  "SerializeCodesText and SerializeCodesBinary are out of sync!");

    template <typename E>
        requires std::is_enum_v<E>
    consteval std::size_t enumSize() noexcept
    {
        return std::meta::enumerators_of(^^E).size();
    }

    template <typename E>
        requires std::is_enum_v<E>
    constexpr std::string_view enumToString(E val) noexcept
    {
        static constexpr auto enumerators = std::define_static_array(std::meta::enumerators_of(^^E));

        template for (constexpr auto e : enumerators)
        {
            if (val == [:e:])
            {
                return std::meta::identifier_of(e);
            }
        }

        return "Unknown code";
    }

} // namespace sas