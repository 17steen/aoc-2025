#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <iostream>
#include <unordered_set>
#include <flux.hpp>
#include <span>
#include <scn/scan.h>

#include "Days.hpp"
#include "Helpers.h"
#include "LongInt.h"

namespace Day06
{
    enum struct Operation
    {
        INVALID,
        PLUS,
        MULTIPLY,
    };

    template <typename Value>
    std::vector<Value> scan_values(const std::string_view str)
    {
        auto values = std::vector<Value>{};
        auto input = scn::ranges::subrange{str};

        while (auto result = scn::scan_value<Value>(input))
        {
            values.push_back(result->value());
            input = result->range();
        }

        return values;
    }

    struct Problem
    {
        std::vector<LongInt> numbers{};
        Operation op{};
    };

    std::string format_as(const Problem& problem)
    {
        auto op = std::string_view{};

        switch (problem.op)
        {
        case Operation::INVALID:
            op = " @ ";
            break;
        case Operation::PLUS:
            op = " + ";
            break;
        case Operation::MULTIPLY:
            op = " * ";
            break;
        }

        return fmt::format("{}", fmt::join(problem.numbers, op));
    }


    using Problems = std::vector<Problem>;

    static Problems Parse(std::string_view input)
    {
        auto result = Problems{};

        auto split_line = [](std::string_view line)
        {
            return scan_values<std::string_view>(line);
        };

        auto not_empty = [](const auto& container)
        {
            return !container.empty();
        };

        auto lines = flux::from_range(input)
                     .split_string("\n")
                     .map(split_line)
                     .filter(not_empty)
                     .to<std::vector>();

        assert(lines.size() >= 2);

        const auto column_length = lines.at(0).size();
        result.resize(column_length);

        for (const auto& line : flux::from_range(lines).take(lines.size() - 1))
        {
            for (int i = 0; auto value : flux::from_range(line).filter_map(
                     aoc::helpers::String_Converter<std::int64_t>()))
            {
                result.at(i).numbers.push_back(value);
                ++i;
            }
        }

        for (int i = 0; auto op : lines.back())
        {
            if (op == "*")
            {
                result.at(i).op = Operation::MULTIPLY;
            }
            else if (op == "+")
            {
                result.at(i).op = Operation::PLUS;
            }
            else
            {
                throw std::runtime_error{"invalid input"};
            }

            ++i;
        }

        return result;
    }

    LongInt Part1(std::span<const Problem> problems)
    {
        auto part1 = LongInt{};
        for (const auto& problem : problems)
        {
            const auto result = flux::from_range(problem.numbers).fold_first([op = problem.op](auto left, auto right)
                {
                    switch (op)
                    {
                    case Operation::PLUS:
                        return left + right;
                    case Operation::MULTIPLY:
                        return left * right;
                    case Operation::INVALID:
                    default:
                        throw std::runtime_error{"invalid"};
                    }
                }
            );

            part1 += result.value();
        }

        return part1;
    }

    LongInt Part2(std::span<const Problem> problems)
    {
        for (const auto& problem: problems)
        {
            auto new_numbers = std::vector<std::string>{};
            new_numbers.resize(64);

            for (const auto& n: problem.numbers)
            {
                auto left_aligned = fmt::format("{:<64}", n);
                if (left_aligned.size() > 64)
                {
                    throw std::runtime_error{"bruh"};
                }

                for (int i = 0; i < 64; ++i)
                {
                    new_numbers.at(i).push_back(left_aligned.at(i));
                }
            }

            fmt::println("{}", new_numbers);

        }
        return 0;
    }

    std::pair<std::string, std::string> Solve(std::string_view input)
    {
        (void)input;

        auto parsed = Parse(input);
        fmt::println("{}", parsed);

        auto part1 = Part1(parsed);
        (void) Part2(parsed);

        return {fmt::to_string(part1), ""};
    } // namespace Day01
}

TEST_CASE("Day6 examples", "[aoc]")
{
    static constexpr auto first_example = R"(123 328  51 64
 45 64  387 23
  6 98  215 314
*   +   *   + )";
    REQUIRE(Day06::Solve(first_example).first == "4277556");
}
