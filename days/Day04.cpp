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
#include <experimental/mdspan>

#include "Days.hpp"

namespace Day04
{
    enum struct Tile
    {
        Empty,
        Pq,
    };

    template <typename T>
    using m2dspan = std::experimental::mdspan<T, std::experimental::extents<
                                                  int, std::experimental::dynamic_extent,
                                                  std::experimental::dynamic_extent>>;

    struct Tiles
    {
    public:
        static Tiles Parse(std::string_view input)
        {
            auto result = Tiles{};

            auto range = scn::ranges::subrange{input};

            auto line_result = scn::scan_value<std::string_view>(range);
            if (!line_result)
            {
                throw std::runtime_error{"invalid input"};
            }

            const auto column_length = line_result->value().length();
            auto row_count = 0;

            do
            {
                ++row_count;

                const auto line = line_result->value();
                if (line.length() != column_length)
                {
                    throw std::runtime_error{"invalid line length"};
                }

                for (const char ch : line)
                {
                    switch (ch)
                    {
                    case '.':
                        result.tiles.push_back(Tile::Empty);
                        break;
                    case '@':
                        result.tiles.push_back(Tile::Pq);
                        break;
                    default:
                        throw std::runtime_error{fmt::format("invalid tile: `{}`", ch)};
                    }
                }


                range = line_result.value();
            }
            while ((line_result = scn::scan_value<std::string_view>(range)).has_value());

            result.grid = m2dspan<Tile>{
                result.tiles.data(), row_count, column_length,
            };

            return result;
        }

        Tiles() = default;

        Tiles(const Tiles& copy): tiles(copy.tiles), grid(tiles.data(), copy.grid.extents())
        {
        }

        Tiles(Tiles&&) = default;

        friend void swap(Tiles& a, Tiles& b) noexcept
        {
            std::swap(a.tiles, b.tiles);
            std::swap(a.grid, b.grid);
        }

        Tiles& operator=(Tiles copy) noexcept
        {
            swap(*this, copy);
            return *this;
        }

    private:
        std::vector<Tile> tiles{};
    public:
        m2dspan<Tile> grid{};
    };


    void Print(m2dspan<const Tile> tile_grid)
    {
        for (int i = 0; i < tile_grid.extent(0); ++i)
        {
            for (int j = 0; j < tile_grid.extent(1); ++j)
            {
                switch (tile_grid[std::array{i, j}])
                {
                case Tile::Empty:
                    fmt::print(".");
                    break;
                case Tile::Pq:
                    fmt::print("@");
                    break;
                }
            }
            fmt::println("");
        }
    }

    struct Remove_Rolls_Results
    {
        Tiles grid_with_removed_rolls{};
        int removed_rolls_count = 0;
    };

    Remove_Rolls_Results Remove_Rolls(const Tiles& tiles)
    {
        auto result = Remove_Rolls_Results{
            .grid_with_removed_rolls = tiles, // copy
            .removed_rolls_count = 0,
        };

        struct Indices
        {
            int i{};
            int j{};
        };

        // clang-format off
        constexpr auto adjacent_directions = std::array{
            Indices{-1, -1}, Indices{-1, 0}, Indices{-1, 1},
            Indices{0, -1},                Indices{0, 1},
            Indices{1, -1}, Indices{1, 0}, Indices{1, 1},
        };
        // clang-format on

        for (int i = 0; i < tiles.grid.extent(0); ++i)
        {
            for (int j = 0; j < tiles.grid.extent(1); ++j)
            {
                const auto this_tile = tiles.grid[std::array{i, j}];
                if (this_tile == Tile::Empty)
                {
                    fmt::print(".");
                    continue;
                }

                auto occupied_tiles = 0;
                for (const auto dir : adjacent_directions)
                {
                    auto ni = i + dir.i;
                    auto nj = j + dir.j;

                    if (ni >= 0 && ni < tiles.grid.extent(0) &&
                        nj >= 0 && nj < tiles.grid.extent(1))
                    {
                        const auto adjacent_tile = tiles.grid[std::array{ni, nj}];
                        if (adjacent_tile == Tile::Pq)
                        {
                            occupied_tiles += 1;
                        }
                    }
                }

                if (occupied_tiles < 4)
                {
                    result.grid_with_removed_rolls.grid[std::array{i, j}] = Tile::Empty;
                    result.removed_rolls_count += 1;
                    fmt::print("x");
                }
                else
                {
                    fmt::print("{}", occupied_tiles);
                }
            }
            fmt::println("");
        }

        return result;
    }


    std::pair<std::string, std::string> Solve(std::string_view input)
    {

        const auto parsed = Tiles::Parse(input);
        Print(parsed.grid);
        fmt::println("\n\n");

        auto result = Remove_Rolls(parsed);
        const auto part1 = result.removed_rolls_count;
        auto part2 = part1;

        while (result.removed_rolls_count != 0)
        {
            result = Remove_Rolls(result.grid_with_removed_rolls);
            part2 += result.removed_rolls_count;
        }
        fmt::println("{}", part2);

        return {fmt::to_string(part1), fmt::to_string(part2)};
    } // namespace Day01
}

TEST_CASE("Day4 examples", "[aoc]")
{
    static constexpr auto first_example = R"(..@@.@@@@.
@@@.@.@.@@
@@@@@.@.@@
@.@@@@..@.
@@.@@@@.@@
.@@@@@@@.@
.@.@.@.@@@
@.@@@.@@@@
.@@@@@@@@.
@.@.@@@.@.)";

    REQUIRE(Day04::Solve(first_example).first == "13");
    REQUIRE(Day04::Solve(first_example).second == "43");
}
