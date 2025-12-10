#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>
#include <fmt/std.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <iostream>
#include <unordered_set>
#include <flux.hpp>
#include <map>
#include <span>
#include <scn/scan.h>

#include "Days.hpp"
#include "Helpers.h"
#include "LongInt.h"

namespace std
{
    template <std::size_t N>
    inline std::string format_as(std::bitset<N> bitset)
    {
        auto result = std::string{"["};
        for (const auto bit : bitset)
        {
            result += bit ? "#" : ".";
        }

        return result + "]";
    }
}

namespace Day10
{
    using Bitset = std::bitset<32>;
    using Lights = Bitset;
    using ButtonsPressed = Bitset;
    using Button = Bitset;

    using IndicesButton = std::vector<int8_t>;

    struct ParseResult
    {
        Lights lights;
        std::vector<Button> button_masks;
        std::vector<IndicesButton> indices_button;
        std::vector<std::int8_t> expected_joltages;
    };

    std::string format_as(const ParseResult& result)
    {
        return fmt::format("Lights: {}, Numbers: {}, Rest: {}", result.lights, result.button_masks,
                           result.expected_joltages);
    }


    [[nodiscard]] ParseResult ParseLine(std::string_view line)
    {
        auto parts = flux::from_range(line)
                     .split_string(" ")
                     .to<std::vector>();

        if (parts.size() < 3)
        {
            throw std::runtime_error{"bad input"};
        }

        auto result = ParseResult{};

        auto first = parts.front();
        first.remove_prefix(1);
        first.remove_suffix(1);

        auto lights = flux::from_range(first).map([](char ch)
        {
            if (ch == '.') return false;
            if (ch == '#') return true;
            throw std::runtime_error{"yep"};
        });
        for (int i = 0; auto light : lights)
        {
            result.lights[i] = light;
            ++i;
        }

        auto middle = std::span{parts.begin() + 1, parts.end() - 1};
        for (auto middle_part : middle)
        {
            middle_part.remove_prefix(1);
            middle_part.remove_suffix(1);


            auto& mask = result.button_masks.emplace_back();
            auto& indices = result.indices_button.emplace_back();

            for (auto index :
                 flux::from_range(middle_part)
                 .split_string(",")
                 .filter_map(aoc::helpers::String_Converter<int>()))
            {
                mask[index] = true;
                indices.push_back(static_cast<std::int8_t>(index));
            }
        }


        auto last = parts.back();
        last.remove_prefix(1);
        last.remove_suffix(1);

        flux::from_range(last).split_string(",")
                              .filter_map(aoc::helpers::String_Converter<int>())
                              .output_to(std::back_inserter(result.expected_joltages));

        return result;
    }

    [[nodiscard]] std::vector<ParseResult> Parse(std::string_view input)
    {
        return flux::from_range(input).split_string("\n").filter(aoc::helpers::Not_Empty()).map(&ParseLine).to<
            std::vector>();
    }


    template<std::size_t N>
    using Array = std::array<int8_t, N>;

    template<std::size_t N>
    struct State2
    {
        Array<N> current_joltage;
        int button_presses = 0;
    };

    template<std::size_t N>
    struct Reference
    {
        std::vector<IndicesButton> indices_button;
        Array<N> expected_joltage;
        std::map<Array<N>, int> state_button_press_map;
    };

    template <std::size_t N>
    int Part2_Impl(Reference<N>& puzzle,
                   const State2<N>& state, int& current_min)
    {
        if (puzzle.state_button_press_map.contains(state.current_joltage))
        {

        }

        if (state.button_presses >= current_min)
        {
            return current_min;
        }

        for (const auto& button : puzzle.indices_button)
        {
            auto cpy = state;
            for (const auto index : button)
            {
                cpy.current_joltage[index] += 1;
            }

            cpy.button_presses += 1;

            if (cpy.current_joltage == puzzle.expected_joltage)
            {
                return cpy.button_presses;
            }

            const bool is_bigger = flux::zip(puzzle.expected_joltage, cpy.current_joltage)
                .any([](auto&& tuple)
                {
                    auto&& [expected, current] = tuple;
                    return current > expected;
                });

            if (is_bigger)
            {
                continue;
            }

            auto result = Part2_Impl<N>(puzzle, cpy, current_min);
            current_min = std::min(result, current_min);

        }

        return current_min;
    }

    int Part2_One_Line(const ParseResult& one_result)
    {
        auto size = one_result.expected_joltages.size();
        auto execute_part_2 = [&]<auto N>(int& result)
        {
            if (N == size)
            {
                auto min = std::numeric_limits<int>::max();
                auto reference = typename Part2Data<N>::Reference{};
                std::ranges::copy_n(one_result.expected_joltages.begin(), N, reference.expected_joltage.begin());
                reference.indices_button = one_result.indices_button;

                auto state = typename Part2Data<N>::State{};

                result = Part2_Impl<N>(reference, state, min);
            }
        };

        auto result = 0;

        [&]<auto... Ns>(std::index_sequence<Ns...>)
        {
            (execute_part_2.operator()<Ns>(result), ...);
        }(std::make_index_sequence<10>{});


        fmt::println("One: {}", result);
        return result;
    }


    struct State
    {
        Lights lights = {};
        ButtonsPressed buttons_pressed = {};
    };

    int Part1_Impl(const ParseResult& one_result, State current_state, int& current_lowest)
    {
        if (static_cast<int>(current_state.buttons_pressed.count()) >= current_lowest)
            return std::numeric_limits<int>::max();

        for (int button_index = 0; const auto mask : one_result.button_masks)
        {
            if (current_state.buttons_pressed[button_index])
            {
                continue;
            }

            auto copy = current_state;
            copy.buttons_pressed[button_index] = true;
            copy.lights = copy.lights ^ mask;

            if (copy.lights == one_result.lights)
            {
                return static_cast<int>(copy.buttons_pressed.count());
            }


            auto result = Part1_Impl(one_result, copy, current_lowest);
            current_lowest = std::min(result, current_lowest);

            ++button_index;
        }

        return current_lowest;
    }

    std::pair<int, int> Solve_One_Line(const ParseResult& one_result)
    {
        auto min = std::numeric_limits<int>::max();
        auto part1 = Part1_Impl(one_result, State{}, min);

        auto part2 = Part2_One_Line(one_result);
        // auto part2 = 0;
        fmt::println("One: {} Two: {}", part1, part2);
        return {part1, part2};
    }


    std::pair<std::string, std::string> Solve(std::string_view input)
    {
        auto parsed = Parse(input);

        auto part1 = 0;
        auto part2 = 0;

        for (const auto [p1, p2] : flux::from_range(parsed).map(&Solve_One_Line))
        {
            part1 += p1;
            part2 += p2;
        }

        fmt::println("{}", parsed);
        return {std::to_string(part1), std::to_string(part2)};
    } // namespace Day01
}

TEST_CASE("Day10 examples", "[aoc]")
{
    static constexpr auto first_example = R"([.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
)";
    REQUIRE(Day10::Solve(first_example).first == "7");
    REQUIRE(Day10::Solve(first_example).second == "33");
}
