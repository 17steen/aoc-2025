#pragma once

#include <optional>
#include <scn/scan.h>

namespace aoc::helpers
{
    template<typename ValueType>
    constexpr static auto String_Converter () -> auto
    {
        return [] (const auto& thing) -> std::optional<ValueType>
        {
            if (auto result = scn::scan_value<ValueType>(thing))
            {
                return result->value();
            }
            return std::nullopt;
        };
    };
}

