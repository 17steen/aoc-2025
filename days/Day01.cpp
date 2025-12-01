#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <fmt/format.h>
#include <scn/scan.h>
#include <span>

#include "Days.hpp"

namespace Day01 {

enum struct Direction {
  LEFT,
  RIGHT,
};

struct Dial_Rotation {
  int ticks{};
  Direction direction{};

  bool operator==(const Dial_Rotation &) const = default;
};

Direction dir_from_ch(char ch) {
  switch (ch) {
  case 'L':
    return Direction::LEFT;
  case 'R':
    return Direction::RIGHT;
  default:;
    throw std::runtime_error{"wtf"};
  }
}

auto Parse(std::string_view input) -> std::vector<Dial_Rotation> {
  auto rotations = std::vector<Dial_Rotation>{};

  for (auto input_range = scn::ranges::subrange{input};
       auto line_result = scn::scan_value<std::string_view>(input_range);
       input_range = line_result.value().range()) {

    auto result = scn::scan<char, int>(line_result.value().value(), "{}{}");
    if (!result) {
      throw std::runtime_error{result.error().msg()};
    }

    auto [ch, ticks] = result->values();

    rotations.push_back(Dial_Rotation{
        .ticks = ticks,
        .direction = dir_from_ch(ch),
    });
  }

  return rotations;
}

static constexpr auto MAX_DIAL = 100;

template <std::integral Int> Int modulo(Int n, Int M) {
  return ((n % M) + M) % M;
}

TEST_CASE("Test modulo", "[modulo]") {
  REQUIRE(modulo(-1, 100) == 99);
  REQUIRE(modulo(100, 100) == 0);
}

int Part1(const std::span<const Dial_Rotation> parsed) {
  struct State {
    int initial = 50;
    int part_1 = 0;
  };

  auto state = State{};

  for (auto [ticks, direction] : parsed) {
    switch (direction) {
    case Direction::LEFT: {
      auto new_value = state.initial - ticks;
      state.initial = modulo(new_value, MAX_DIAL);
    } break;
    case Direction::RIGHT: {
      auto new_value = state.initial + ticks;
      state.initial = modulo(new_value, MAX_DIAL);
    } break;
    }

    if (state.initial == 0) {
      state.part_1 += 1;
    }
  }

  return state.part_1;
}

int Part2(const std::span<const Dial_Rotation> parsed) {
  struct State {
    int initial = 50;
    int part_2 = 0;
  };

  auto state = State{};

  for (auto [ticks, direction] : parsed) {
    for (int tick = 0; tick < ticks; ++tick) {
      switch (direction) {
      case Direction::LEFT: {
        auto new_value = state.initial - 1;
        state.initial = modulo(new_value, MAX_DIAL);
      } break;
      case Direction::RIGHT: {
        auto new_value = state.initial + 1;
        state.initial = modulo(new_value, MAX_DIAL);
      } break;
      }

      if (state.initial == 0) {
        state.part_2 += 1;
      }
    }
  }

  return state.part_2;
}

std::pair<int, int> Solve(std::string_view input) {

  auto parsed = Parse(input);

  auto part1 = Part1(parsed);
  auto part2= Part2(parsed);

  return {part1, part2};
}
} // namespace Day01

TEST_CASE("Day1 part 1", "[aoc]") {
  static constexpr auto one_line = R"(R727)";

  REQUIRE(Day01::Parse(one_line) == std::vector{Day01::Dial_Rotation{
                                        .ticks = 727,
                                        .direction = Day01::Direction::RIGHT,
                                    }});

  static constexpr auto two_lines = R"(R727
L547
)";

  REQUIRE(Day01::Parse(two_lines) ==
          std::vector{Day01::Dial_Rotation{
                          .ticks = 727,
                          .direction = Day01::Direction::RIGHT,
                      },
                      Day01::Dial_Rotation{
                          .ticks = 547,
                          .direction = Day01::Direction::LEFT,
                      }});

  static constexpr auto part1_example = R"(L68
L30
R48
L5
R60
L55
L1
L99
R14
L82)";

  REQUIRE(Day01::Solve(part1_example) == std::pair{3, 6});
}
