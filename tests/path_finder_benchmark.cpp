
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include "path_find.hpp"

#include <unordered_map>
#include <vector>
#include <random>

namespace {

Grid2 make_benchmark_grid(int width, int height) {
    return Grid2{
        Vector<float, float>{0.0f, 0.0f},
        Vector<float, float>{1.0f, 1.0f},
        Vector2I{width, height}
    };
}

}

TEST_CASE("Pathfinder benchmarks", "[.benchmark]") {
    SECTION("64x64 open grid") {
        Grid2 grid = make_benchmark_grid(64, 64);
        Pathfinder pathfinder{grid};

        const Vector2I start{0, 0};
        const Vector2I goal{63, 63};

        BENCHMARK("A* cardinal") {
            return pathfinder.a_star_cardinal(start, goal);
        };

        BENCHMARK("Dijkstra cardinal") {
            return pathfinder.dijkstra_cardinal(start, goal);
        };

        BENCHMARK("A* diagonal") {
            return pathfinder.a_star_diagonal(start, goal);
        };

        BENCHMARK("Dijkstra diagonal") {
            return pathfinder.dijkstra_diagonal(start, goal);
        };
    }

    SECTION("128x128 open grid") {
        Grid2 grid = make_benchmark_grid(128, 128);
        Pathfinder pathfinder{grid};

        const Vector2I start{0, 0};
        const Vector2I goal{127, 127};

        BENCHMARK("A* cardinal") {
            return pathfinder.a_star_cardinal(start, goal);
        };

        BENCHMARK("Dijkstra cardinal") {
            return pathfinder.dijkstra_cardinal(start, goal);
        };

        BENCHMARK("A* diagonal") {
            return pathfinder.a_star_diagonal(start, goal);
        };

        BENCHMARK("Dijkstra diagonal") {
            return pathfinder.dijkstra_diagonal(start, goal);
        };
    }

    SECTION("256x256 open grid") {
        Grid2 grid = make_benchmark_grid(256, 256);
        Pathfinder pathfinder{grid};

        const Vector2I start{0, 0};
        const Vector2I goal{255, 255};

        BENCHMARK("A* cardinal") {
            return pathfinder.a_star_cardinal(start, goal);
        };

        BENCHMARK("Dijkstra cardinal") {
            return pathfinder.dijkstra_cardinal(start, goal);
        };

        BENCHMARK("A* diagonal") {
            return pathfinder.a_star_diagonal(start, goal);
        };

        BENCHMARK("Dijkstra diagonal") {
            return pathfinder.dijkstra_diagonal(start, goal);
        };
    }
}

TEST_CASE("Pathfinder benchmark - heavily obstructed grid", "[obstructed][.benchmark]") {
    constexpr int GRID_SIZE = 256;
    constexpr float OBSTACLE_PROBABILITY = 0.30f;

    const Vector2I start{0, 0};
    const Vector2I goal{GRID_SIZE - 1, GRID_SIZE - 1};

    Grid2 grid{
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        Vector2I{GRID_SIZE, GRID_SIZE}
    };

    std::unordered_map<Vector2I, float> weights;

    std::mt19937 random_engine{42};
    std::uniform_real_distribution<float> distribution{0.0f, 1.0f};

    // Generate a reproducible obstacle layout.
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            const Vector2I position{x, y};

            if (position == start || position == goal) {
                continue;
            }

            if (distribution(random_engine) < OBSTACLE_PROBABILITY) {
                weights.emplace(position, Pathfinder::UNPASSABLE);
            }
        }
    }

    const Pathfinder pathfinder{weights, grid};

    BENCHMARK("A* cardinal - 30% obstacles") {
        return pathfinder.a_star_cardinal(start, goal);
    };

    BENCHMARK("Dijkstra cardinal - 30% obstacles") {
        return pathfinder.dijkstra_cardinal(start, goal);
    };

    BENCHMARK("A* diagonal - 30% obstacles") {
        return pathfinder.a_star_diagonal(start, goal);
    };

    BENCHMARK("Dijkstra diagonal - 30% obstacles") {
        return pathfinder.dijkstra_diagonal(start, goal);
    };
}