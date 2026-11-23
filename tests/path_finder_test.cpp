
#include <catch2/catch_test_macros.hpp>

#include "path_find.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <unordered_map>
#include <vector>

namespace {

using Position = Vector2I;
using Weights = std::unordered_map<Position, float>;

Grid2 make_grid(int width, int height) {
    return Grid2{
        Vector<float, float>{0.0f, 0.0f},
        Vector<float, float>{1.0f, 1.0f},
        Position{width, height}
    };
}

float path_cost(
    const std::vector<Position>& path,
    const Weights& weights = {}
) {
    float cost = 0.0f;

    // The start cell is not charged.
    for (std::size_t i = 1; i < path.size(); ++i) {
        const auto found = weights.find(path[i]);

        cost += found == weights.end() ? 1.0f : found->second;
    }

    return cost;
}

void check_path(
    const std::vector<Position>& path,
    const Position& start,
    const Position& goal,
    bool diagonal
) {
    REQUIRE_FALSE(path.empty());
    REQUIRE(path.back() == start);
    REQUIRE(path.front() == goal);

    for (std::size_t i = 1; i < path.size(); ++i) {
        const int dx = std::abs(path[i].x - path[i - 1].x);
        const int dy = std::abs(path[i].y - path[i - 1].y);

        REQUIRE(dx <= 1);
        REQUIRE(dy <= 1);
        REQUIRE(dx + dy > 0);

        if (!diagonal) {
            REQUIRE(dx + dy == 1);
        }
    }
}

}

TEST_CASE("Pathfinder cardinal movement", "[pathfinder][cardinal]") {
    Grid2 grid = make_grid(10, 10);
    // Grid2 grid = make_grid(3, 3);
    Pathfinder pathfinder{grid};

    SECTION("Finds a straight horizontal path") {
        const Position start{1, 2};
        const Position goal{6, 2};

        const std::vector<Position> path =
            pathfinder.a_star_cardinal(start, goal);

        check_path(path, start, goal, false);
        REQUIRE(path.size() == 6);
        REQUIRE(path_cost(path) == 5.0f);
    }

    SECTION("Finds a straight vertical path") {
        const Position start{3, 1};
        const Position goal{3, 7};

        const std::vector<Position> path =
            pathfinder.a_star_cardinal(start, goal);

        check_path(path, start, goal, false);
        REQUIRE(path.size() == 7);
        REQUIRE(path_cost(path) == 6.0f);
    }

    SECTION("Finds a path between arbitrary positions") {
        const Position start{1, 1};
        const Position goal{7, 6};

        const std::vector<Position> path =
            pathfinder.a_star_cardinal(start, goal);

        check_path(path, start, goal, false);

        // Manhattan distance: |7 - 1| + |6 - 1| = 11.
        REQUIRE(path_cost(path) == 11.0f);
        REQUIRE(path.size() == 12);
    }

    SECTION("Start and goal are the same") {
        const Position start{4, 4};

        const std::vector<Position> path =
            pathfinder.a_star_cardinal(start, start);

        REQUIRE(path.size() == 1);
        REQUIRE(path.back() == start);
    }
}

TEST_CASE("Pathfinder diagonal movement", "[pathfinder][diagonal]") {
    Grid2 grid = make_grid(10, 10);
    Pathfinder pathfinder{grid};

    SECTION("Finds a diagonal path") {
        const Position start{1, 1};
        const Position goal{6, 6};

        const std::vector<Position> path =
            pathfinder.a_star_diagonal(start, goal);

        check_path(path, start, goal, true);

        // This checks the route length when diagonal movement
        // is allowed, independently of its movement cost.
        REQUIRE(path.size() == 6);
    }

    SECTION("Can combine diagonal and cardinal movement") {
        const Position start{1, 1};
        const Position goal{6, 3};

        const std::vector<Position> path =
            pathfinder.a_star_diagonal(start, goal);

        check_path(path, start, goal, true);
        REQUIRE(path.size() == 6);
    }

    SECTION("Start and goal are the same") {
        const Position start{4, 4};

        const std::vector<Position> path =
            pathfinder.a_star_diagonal(start, start);

        REQUIRE(path.size() == 1);
        REQUIRE(path.back() == start);
    }
}

TEST_CASE("Pathfinder respects impassable cells", "[pathfinder][obstacles]") {
    Grid2 grid = make_grid(7, 7);

    Weights weights;

    // A vertical wall, with one opening at y = 5.
    for (int y = 0; y < 7; ++y) {
        if (y != 5) {
            weights.emplace(Position{3, y}, Pathfinder::UNPASSABLE);
        }
    }

    Pathfinder pathfinder{weights, grid};

    const Position start{1, 3};
    const Position goal{5, 3};

    const std::vector<Position> path =
        pathfinder.a_star_cardinal(start, goal);

    check_path(path, start, goal, false);

    for (const Position& position : path) {
        const auto found = weights.find(position);

        REQUIRE((
            found == weights.end() ||
            found->second != Pathfinder::UNPASSABLE
        ));
    }

    // The route must go through the opening.
    REQUIRE(
        std::find(path.begin(), path.end(), Position{3, 5}) != path.end()
    );
}

TEST_CASE("Pathfinder uses traversal weights", "[pathfinder][weights]") {
    Grid2 grid = make_grid(5, 3);

    Weights weights;

    // Make the direct route expensive.
    weights.emplace(Position{1, 1}, 10.0f);
    weights.emplace(Position{2, 1}, 10.0f);
    weights.emplace(Position{3, 1}, 10.0f);

    Pathfinder pathfinder{weights, grid};

    const Position start{0, 1};
    const Position goal{4, 1};

    const std::vector<Position> path =
        pathfinder.a_star_cardinal(start, goal);

    check_path(path, start, goal, false);

    // The optimal route goes around the expensive cells.
    REQUIRE(path_cost(path, weights) == 6.0f);
}

TEST_CASE("Pathfinder A* agrees with Dijkstra", "[pathfinder][optimality]") {
    Grid2 grid = make_grid(20, 20);

    Weights weights;

    // Deterministic obstacles and higher-cost cells.
    for (int y = 0; y < 20; ++y) {
        if (y != 10) {
            weights.emplace(Position{8, y}, Pathfinder::UNPASSABLE);
        }
    }

    for (int x = 10; x < 16; ++x) {
        weights.emplace(Position{x, 6}, 4.0f);
    }

    Pathfinder pathfinder{weights, grid};

    const Position start{1, 1};
    const Position goal{18, 18};

    SECTION("Cardinal paths have equal optimal cost") {
        const std::vector<Position> astar =
            pathfinder.a_star_cardinal(start, goal);

        const std::vector<Position> dijkstra =
            pathfinder.dijkstra_cardinal(start, goal);

        check_path(astar, start, goal, false);
        check_path(dijkstra, start, goal, false);

        REQUIRE(
            path_cost(astar, weights) ==
            path_cost(dijkstra, weights)
        );
    }

    SECTION("Diagonal paths have equal optimal cost") {
        const std::vector<Position> astar =
            pathfinder.a_star_diagonal(start, goal);

        const std::vector<Position> dijkstra =
            pathfinder.dijkstra_diagonal(start, goal);

        check_path(astar, start, goal, true);
        check_path(dijkstra, start, goal, true);

        REQUIRE(
            path_cost(astar, weights) ==
            path_cost(dijkstra, weights)
        );
    }
}

TEST_CASE("Pathfinder reports unreachable goals", "[pathfinder][unreachable]") {
    Grid2 grid = make_grid(5, 5);

    Weights weights;

    // Completely block the middle column.
    for (int y = 0; y < 5; ++y) {
        weights.emplace(Position{2, y}, Pathfinder::UNPASSABLE);
    }

    Pathfinder pathfinder{weights, grid};

    REQUIRE_THROWS_AS(
        pathfinder.a_star_cardinal(Position{0, 2}, Position{4, 2}),
        std::out_of_range
    );

    REQUIRE_THROWS_AS(
        pathfinder.dijkstra_cardinal(Position{0, 2}, Position{4, 2}),
        std::out_of_range
    );
}
