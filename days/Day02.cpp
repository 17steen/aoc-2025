#include <charconv>
#include <catch2/catch_test_macros.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <scn/scan.h>
#include <flux.hpp>
#include <span>
#include <unordered_set>

#include "Days.hpp"

namespace Day02 {

  struct Range
  {
    std::int64_t begin{};
    std::int64_t end{};

    constexpr bool operator==(const Range&) const= default;
  };

auto Parse(std::string_view input) -> std::vector<Range> {
  auto result = flux::from_range(input).split_string(",").filter_map([](std::string_view range) -> std::optional<Range>
  {
    if (auto scanned = scn::scan<std::int64_t, std::int64_t>(range, "{}-{}"))
    {
      auto [begin, end] = scanned->values();
      return Range{.begin = begin, .end = end, };
    }
    else
    {
      fmt::println(stderr, "Failed to parse: {}", range);
      return std::nullopt;
    }
  }).to<std::vector>();

  return result;
}

template<Selected_Part Part = Selected_Part::Part1>
std::vector<std::int64_t> Compute_Invalid_Ids(const Range range)
{
  auto invalid_ids = std::vector<std::int64_t>{};

  // big enough for any 64 bit int
  auto buffer = std::array<char, 20>{};

  for (std::int64_t current = range.begin; current <= range.end; ++current)
  {
    auto [end, result] = std::to_chars(buffer.begin(), buffer.end(), current, 10);
    if (result != std::errc{})
    {
      throw std::runtime_error{"fuck"};
    }
    if (end == buffer.end())
    {
      throw std::runtime_error{"fuck2"};
    }

    const auto str = std::string_view{buffer.begin(), end};
    if constexpr (Part == Selected_Part::Part2) {
      const auto half = str.length() / 2;
      for (int repeat_size = 1; repeat_size <= half; ++repeat_size)
      {
        if (str.length() % repeat_size != 0)
        {
          continue;
        }

        const auto first_section = str.substr(0, repeat_size);

        bool match = false;
        for (int section_start = repeat_size; section_start != str.length(); section_start += repeat_size)
        {
          const auto next_section = str.substr(section_start, repeat_size);

          if (next_section == first_section)
          {
            match = true;
          }
          else
          {
            match = false;
            break;
          }
        }

        if (match)
        {
          invalid_ids.push_back(current);
        }

      }
    }
    else
    {
      if (str.length() % 2 != 0)
      {
        continue;
      }

      const auto first_half = str.substr(0, str.length() / 2);
      const auto second_half = str.substr(str.length()/2);

      if (first_half == second_half)
      {
        invalid_ids.push_back(current);
      }
    }


  }

  return invalid_ids;
}

std::string Part1(const std::span<const Range> parsed) {
  auto sum = __int128_t{};
  for (const auto range: parsed)
  {
    for (const auto invalid_id: Compute_Invalid_Ids(range))
    {
      sum += invalid_id;
    }
  }

  return fmt::format("{}", sum);
}

std::string Part2(const std::span<const Range> parsed) {
  const auto then = std::chrono::system_clock::now();

  auto unique_invalid_ids = std::unordered_set<std::int64_t>{};
  for (const auto range: parsed)
  {
    for (const auto invalid_id: Compute_Invalid_Ids<true>(range))
    {
      unique_invalid_ids.emplace(invalid_id);
    }
  }

  auto sum = __int128_t{};
  for (const auto invalid_id: unique_invalid_ids)
  {
    sum += invalid_id;
  }


  auto elapsed = std::chrono::system_clock::now() - then;
  using floating_millis = std::chrono::duration<double, std::milli>;
  fmt::println("Solved in {}", std::chrono::duration_cast<floating_millis>(elapsed));

  return fmt::format("{}", sum);
}


std::pair<std::string, std::string> Solve(std::string_view input) {

  auto parsed = Parse(input);

  for (const auto range: parsed)
  {
    fmt::println("Range: [{} - {}]", range.begin, range.end);
  }

  auto part1 = Part1(parsed);
  auto part2 = Part2(parsed);

  return {part1, part2};
}
} // namespace Day01

TEST_CASE("Day2 examples", "[aoc]") {
  static constexpr auto one_line = R"(11-22,95-115)";

  REQUIRE(Day02::Parse(one_line) == std::vector{Day02::Range{11, 22}, Day02::Range{95,115}});

  REQUIRE(Day02::Compute_Invalid_Ids(Day02::Range{11, 22}) == std::vector<std::int64_t>{11, 22});

  static constexpr auto part1_example = "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124";

  REQUIRE(Day02::Part1(Day02::Parse(part1_example)) == "1227775554");


  static constexpr auto part2_example = "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124";
  REQUIRE(Day02::Part2(Day02::Parse(part2_example)) == "4174379265");

  REQUIRE(Day02::Compute_Invalid_Ids<true>(Day02::Range{95, 115}) == std::vector<std::int64_t>{99, 111});
  REQUIRE(Day02::Compute_Invalid_Ids<true>(Day02::Range{824824821, 824824827}) == std::vector<std::int64_t>{824824824});
  REQUIRE(Day02::Compute_Invalid_Ids<true>(Day02::Range{2121212118, 2121212124}) == std::vector<std::int64_t>{2121212121});

}
