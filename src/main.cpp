#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include <scn/scan.h>

#include "../days/Days.hpp"

#include <fmt/format.h>
#include <fmt/std.h>

namespace std
{
    template<typename First, typename Second>
    inline auto format_as(const std::pair<First, Second>& pair) -> std::string
    {
        return fmt::format("{{first: {}, second: {}}}", pair.first, pair.second);
    }
}

static const auto day_map =
    std::unordered_map<int, std::function<std::string(std::string_view)>>{
        {
            1, [](std::string_view sv) { return fmt::to_string(Day01::Solve(sv)); },
        },
        {
            2, [](std::string_view sv) { return fmt::to_string(Day02::Solve(sv)); },
        },
    };

std::string read_all_input()
{
    auto result = std::string{};
    std::getline(std::cin, result, '\0');
    return result;
}

int main(int, char**)
{
    if (auto day_number = scn::prompt<int>("Enter day number: ", "{}"))
    {
        if (day_map.contains(day_number->value()))
        {
            auto input = read_all_input();

            auto func = day_map.at(day_number->value());
            auto result = func(input);
            fmt::println("Result: {}", result);
        }
        else
        {
            fmt::println(stderr, "Unimplemented day number");
        }
    }
}
