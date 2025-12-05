#include <charconv>
#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <scn/scan.h>
#include <scn/istream.h>
#include <scn/ranges.h>
#include <flux.hpp>
#include <iostream>
#include <span>
#include <unordered_set>
#include <experimental/mdspan>

#include "Days.hpp"

namespace Day05
{
    using Id = std::int64_t;

    struct Range
    {
        Id begin{};
        Id end{};

        [[nodiscard]] Id Amount_Of_Values() const
        {
            return end - begin + 1;
        }

        [[nodiscard]] bool Includes(Id value) const
        {
            return value >= begin and value <= end;
        }

        [[nodiscard]] static std::pair<Range, Range> Disintersect(Range left, Range right)
        {
            auto [a, b] = Disintersect_Impl(left, right);

            auto [new_right, new_left] = Disintersect_Impl(b, a);

            return {new_left, new_right};

        }

        [[nodiscard]] static std::pair<Range, Range> Disintersect_Impl(Range left, Range right);

        bool operator==(const Range&) const = default;
    };

    using Ranges = std::vector<Range>;

    std::string format_as(Range range)
    {
        return fmt::format("[{}, {}]", range.begin, range.end);
    }

    [[nodiscard]] std::pair<Range, Range> Range::Disintersect_Impl(Range left, Range right)
    {
        if (left.Amount_Of_Values() < 1 or right.Amount_Of_Values() < 1)
        {
            fmt::println("{} and {} cannot be processed", left, right);
            return {left, right};
        }

        if (right.begin >= left.begin && right.begin <= left.end)
        {
            right.begin = left.begin + 1;
            // the begining of the right range is contained in the left range
        }

        if (right.end <= left.end && right.end >= left.begin)
        {
            right.end = left.begin - 1;
        }

        if (right.Amount_Of_Values() < 1)
        {
            fmt::println("{} is a negative range", right);
        }

        return {left, right};
    }

    struct Result
    {
        Ranges ranges;
        std::vector<Id> ids;

        bool operator==(const Result&) const = default;
    };

    template<typename ValueType>
    constexpr static auto to_number() -> auto
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

    Result Parse(const std::string_view input)
    {

        auto result = Result{};
        auto splat = flux::split_string(input, "\n\n").take(2).to<std::vector>();

        auto ranges_str = splat.at(0);
        auto ids = splat.at(1);

        result.ranges = flux::split_string(ranges_str, "\n")
                        .filter_map([](std::string_view range_str) -> std::optional<Range>
                        {
                            if (range_str.empty())
                                return std::nullopt;

                            auto arr = std::array<Id, 2>{};
                            flux::split_string(range_str, "-").filter_map(to_number<Id>()).take(2).output_to(arr.begin());
                            auto [begin, end] = arr;
                            return Range{
                                .begin = begin,
                                .end = end,
                            };
                        }).to<std::vector>();

        result.ids = flux::split_string(ids, "\n").filter_map(to_number<Id>()).to<std::vector>();

        return result;
    }

    std::int64_t Part1(const Result& result)
    {
        auto fresh_count = 0;
        for (const auto id: result.ids)
        {
            for (const auto range: result.ranges)
            {
                if (range.Includes(id))
                {
                    fresh_count += 1;
                    break;
                }
            }
        }

        return fresh_count;
    }

    Ranges Uniquify_Ranges(Ranges ranges)
    {
        while (true)
        {
            bool changed = false;
            for (int i = 0; i < static_cast<int>(ranges.size()); ++i)
            {
                auto& first_range = ranges.at(i);
                for (int j = 0; j < static_cast<int>(ranges.size()); ++j)
                {
                    if (i == j) continue;

                    auto& second_range = ranges.at(j);

                    auto disintersected = Range::Disintersect(first_range, second_range);

                    if (std::pair{first_range, second_range} != disintersected)
                    {
                        changed = true;
                        first_range = disintersected.first;
                        second_range = disintersected.second;
                    }
                }
            }

            if (changed == false)
            {
                break;
            }
        }

        [[maybe_unused]] const bool erased = std::erase_if(ranges, [](Range range)
        {
            return range.Amount_Of_Values() < 1;
        });

        return ranges;
    }

    std::int64_t Part2(Ranges ranges)
    {
        auto unique_ranges = Uniquify_Ranges(std::move(ranges));
        return flux::from_range(unique_ranges).map(&Range::Amount_Of_Values).sum();
    }

    std::pair<std::string, std::string> Solve(std::string_view input)
    {
        auto parsed = Parse(input);

        const auto part1 = Part1(parsed);
        const auto part2 = Part2(std::move(parsed.ranges));


        return {fmt::to_string(part1), fmt::to_string(part2)};
    } // namespace Day01
}

TEST_CASE("Day5 examples", "[aoc]")
{
    static constexpr auto first_example = R"(3-5
10-14
16-20
12-18

1
5
8
11
17
32)";

    REQUIRE(Day05::Parse(first_example) == Day05::Result{
            .ranges = {
                Day05::Range{
                3, 5
                },
                Day05::Range{
            10, 14
            },
            Day05::Range{
            16, 20
            },
            Day05::Range{
            12, 18
            },
            },
            .ids = {
            1, 5, 8, 11, 17, 32,
            },
            });

    REQUIRE(Day05::Solve(first_example).first == "3");
    REQUIRE(Day05::Solve(first_example).second == "14");
}
