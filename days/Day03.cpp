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

    constexpr auto AMOUNT_OF_BATTERIES_PART1 = 2;
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

    std::pair<std::string, std::string> Solve(std::string_view input)
    {
        auto part1 = std::int64_t{0};
        auto part2 = std::int64_t{0};

        for (auto range = scn::ranges::subrange{input};
             const auto line_result = scn::scan_value<std::string_view>(range);
             range = line_result.value())
        {
            part1 += Solve_One_Row<AMOUNT_OF_BATTERIES_PART1>(line_result->value());
            part2 += Solve_One_Row<AMOUNT_OF_BATTERIES_PART2>(line_result->value());
        }

        return {fmt::to_string(part1), fmt::to_string(part2)};
    } // namespace Day01
}

TEST_CASE("Day3 examples", "[aoc]")
{
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART1>("987654321111111") == 98);
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART1>("811111111111119") == 89);
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART1>("234234234234278") == 78);
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART1>("818181911112111") == 92);

    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART2>("987654321111111") == 987654321111);
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART2>("811111111111119") == 811111111119);
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART2>("234234234234278") == 434234234278);
    REQUIRE(Day03::Solve_One_Row<Day03::AMOUNT_OF_BATTERIES_PART2>("818181911112111") == 888911112111);


    static constexpr auto first_example = R"(987654321111111
811111111111119
234234234234278
818181911112111)";

    REQUIRE(Day03::Solve(first_example).first == "357");
}
