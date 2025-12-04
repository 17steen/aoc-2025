#include <charconv>
#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <scn/scan.h>
#include <scn/istream.h>
#include <scn/ranges.h>
#include <flux.hpp>
#include <span>
#include <unordered_set>

#include "Days.hpp"

namespace Day03
{
    int Part1_One_Row(const std::string_view input)
    {
        const auto all_but_last = input.substr(0, input.length() - 1);
        const auto it = std::ranges::max_element(all_but_last);
        if (it == all_but_last.end())
        {
            throw std::runtime_error{"bro what"};
        }

        const auto index = std::ranges::distance(all_but_last.begin(), it);

        const auto first_digit = *it - '0';
        const auto rest = input.substr(index + 1);

        const auto second_digit = std::ranges::max(rest) - '0';

        return first_digit * 10 + second_digit;
    }

    constexpr auto AMOUNT_OF_BATTERIES_PART1 = 1;
    constexpr auto AMOUNT_OF_BATTERIES_PART2 = 12;


    template<std::size_t Amount_Of_Batteries = AMOUNT_OF_BATTERIES_PART1>
    std::int64_t Solve_One_Row(const std::string_view input)
    {
        assert(input.length() >= Amount_Of_Batteries);
        auto resulting_number = std::int64_t{0};
        auto rest = input;
        for (int i = Amount_Of_Batteries; i > 0; --i)
        {
            const auto all_but_last = rest.substr(0, rest.length() - i + 1);
            const auto it = std::ranges::max_element(all_but_last);

            if (it == all_but_last.end())
            {
                throw std::runtime_error{"bro what"};
            }

            const auto index = std::ranges::distance(all_but_last.begin(), it);

            const auto digit = *it - '0';
            resulting_number *= 10;
            resulting_number += digit;
            rest = rest.substr(index + 1);
        }


        return resulting_number;
    }

    int Part1(const std::string_view input)
    {
        auto sum = 0;
        for (auto range = scn::ranges::subrange{input};
             const auto line_result = scn::scan_value<std::string_view>(range);
             range = line_result.value())
        {
            sum += Part1_One_Row(line_result->value());
        }

        return sum;
    }

    std::int64_t Part2(const std::string_view input)
    {
        auto sum = std::int64_t{0};
        for (auto range = scn::ranges::subrange{input};
             const auto line_result = scn::scan_value<std::string_view>(range);
             range = line_result.value())
        {
            sum += Solve_One_Row(line_result->value());
        }

        return sum;
    }


    std::pair<std::string, std::string> Solve(std::string_view input)
    {
        const auto part1 = Part1(input);
        const auto part2 = Part2(input);
        return {fmt::to_string(part1), fmt::to_string(part2)};
    } // namespace Day01
}

TEST_CASE("Day3 examples", "[aoc]")
{
    REQUIRE(Day03::Part1_One_Row("987654321111111") == 98);
    REQUIRE(Day03::Part1_One_Row("811111111111119") == 89);
    REQUIRE(Day03::Part1_One_Row("234234234234278") == 78);
    REQUIRE(Day03::Part1_One_Row("818181911112111") == 92);

    REQUIRE(Day03::Solve_One_Row("987654321111111") == 987654321111);
    REQUIRE(Day03::Solve_One_Row("811111111111119") == 811111111119);
    REQUIRE(Day03::Solve_One_Row("234234234234278") == 434234234278);
    REQUIRE(Day03::Solve_One_Row("818181911112111") == 888911112111);


    static constexpr auto first_example = R"(987654321111111
811111111111119
234234234234278
818181911112111)";

    REQUIRE(Day03::Part1(first_example) == 357);
}
