#include <catch2/catch_test_macros.hpp>
#include "grid.hpp"

TEST_CASE("Grid intializer", "[Grid]"){
    Grid g{Vector{0, 0}, Vector{1, 1}};
    
    REQUIRE(g.size == Vector{-1, -1});

    REQUIRE(g.get_global_position(Vector{3, 4}) == Vector{3, 4});
    REQUIRE(g.get_global_position(Vector{5, 6}) != Vector{3, 4});

    Grid h{Vector{3.5f, 4.2f}, Vector{7.8f, 9.1f}};

    REQUIRE(h.get_global_position(Vector{0, 0}) == Vector{3.5f, 4.2f});
    REQUIRE(h.get_global_position(Vector{1, 1}) == Vector{11.3f, 13.3f});
}

TEST_CASE("Grid is inside", "[Grid]"){
    using Vector3 = Vector<float, float, float>;
    Grid g{Vector3{}, Vector3{}, Vector3{10, -1, 5}};

    REQUIRE(g.is_inside(Vector3{}));
    REQUIRE(g.is_inside(Vector3{9, 100, 4}));
    REQUIRE(!g.is_inside(Vector3{10, 0, 0}));
    REQUIRE(!g.is_inside(Vector3{0, 0, 5}));
    REQUIRE(!g.is_inside(Vector3{-1, 0, 0}));
}

TEST_CASE("Grid get_cardinal_neighbors", "[Grid]") {
    using grid_type = Grid<int, int>;
    SECTION("Returns four neighbors for an interior position") {
        grid_type grid;
        grid.size = {10, 10};

        const grid_type::Index position{5, 5};

        const std::vector<grid_type::Index> expected{
            {4, 5},
            {6, 5},
            {5, 4},
            {5, 6}
        };

        const std::vector<grid_type::Index> actual =
            grid.get_cardinal_neighbors(position);

        REQUIRE(actual.size() == expected.size());

        for (const auto& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor) != actual.end());
        }
    }

    SECTION("Excludes neighbors outside bounded grid") {
        grid_type grid;
        grid.size = {10, 10};

        const grid_type::Index position{0, 0};

        const std::vector<grid_type::Index> expected{
            {1, 0},
            {0, 1}
        };

        const std::vector<grid_type::Index> actual =
            grid.get_cardinal_neighbors(position);

        REQUIRE(actual.size() == expected.size());

        for (const auto& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor) != actual.end());
        }
    }

    SECTION("Supports unbounded axes") {
        grid_type grid;
        grid.size = {-1, 10};

        const grid_type::Index position{0, 5};

        const std::vector<grid_type::Index> actual =
            grid.get_cardinal_neighbors(position);

        REQUIRE(actual.size() == 4);
    }
}

TEST_CASE("Grid get_diagonal_neighbors", "[Grid]") {
    using grid_type = Grid<int, int>;

    SECTION("Returns eight neighbors for an interior position") {
        grid_type grid;
        grid.size = {10, 10};

        const grid_type::Index position{5, 5};

        const std::vector<grid_type::Index> expected{
            {4, 4}, {4, 5}, {4, 6},
            {5, 4},          {5, 6},
            {6, 4}, {6, 5}, {6, 6}
        };

        const std::vector<grid_type::Index> actual =
            grid.get_diagonal_neighbors(position);

        REQUIRE(actual.size() == expected.size());

        for (const auto& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor) != actual.end());
        }
    }

    SECTION("Excludes neighbors outside bounded grid") {
        grid_type grid;
        grid.size = {10, 10};

        const grid_type::Index position{0, 0};

        const std::vector<grid_type::Index> expected{
            {0, 1},
            {1, 0},
            {1, 1}
        };

        const std::vector<grid_type::Index> actual =
            grid.get_diagonal_neighbors(position);

        REQUIRE(actual.size() == expected.size());

        for (const auto& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor) != actual.end());
        }
    }

    SECTION("Supports unbounded grids") {
        grid_type grid;
        grid.size = {-1, -1};

        const grid_type::Index position{0, 0};

        const std::vector<grid_type::Index> actual =
            grid.get_diagonal_neighbors(position);

        REQUIRE(actual.size() == 8);
    }
}

TEST_CASE("Grid get_double_neighbors in 3D", "[Grid]") {
    Grid<int, int, int> grid;
    grid.size = {10, 10, 10};

    const Grid<int, int, int>::Index position{5, 5, 5};

    const std::vector<Grid<int, int, int>::Index> actual =
        grid.get_diagonal_neighbors(position);

    // 3^3 - 1 = 26 neighbors.
    REQUIRE(actual.size() == 26);
}

TEST_CASE("Grid get_index", "[Grid]") {
    using grid_type = Grid<float, float>;

    SECTION("Returns the origin index") {
        grid_type grid;
        grid.origin = {0, 0.0f};
        grid.basis = {10.0f, 10.0f};

        const Vector<float, float> position{0.0f, 0.0f};

        const grid_type::Index expected{0, 0};

        REQUIRE(grid.get_index(position) == expected);
    }

    SECTION("Returns the correct index for positive positions") {
        grid_type grid;
        grid.origin = {0.0f, 0.0f};
        grid.basis = {10.0f, 10.0f};

        const Vector<float, float> position{25.0f, 35.0f};

        const grid_type::Index expected{2, 3};

        REQUIRE(grid.get_index(position) == expected);
    }

    SECTION("Handles positions inside the first cell") {
        grid_type grid;
        grid.origin = {0.0f, 0.0f};
        grid.basis = {10.0f, 10.0f};

        const Vector<float, float> position{9.9f, 0.1f};

        const grid_type::Index expected{0, 0};

        REQUIRE(grid.get_index(position) == expected);
    }

    SECTION("Handles negative positions using floor") {
        grid_type grid;
        grid.origin = {0.0f, 0.0f};
        grid.basis = {10.0f, 10.0f};

        const Vector<float, float> position{-2.0f, 5.0f};

        const grid_type::Index expected{-1, 0};

        REQUIRE(grid.get_index(position) == expected);
    }

    SECTION("Accounts for a non-zero center") {
        grid_type grid;
        grid.origin = {100.0f, 200.0f};
        grid.basis = {10.0f, 20.0f};

        const Vector<float, float> position{125.0f, 250.0f};

        const grid_type::Index expected{2, 2};

        REQUIRE(grid.get_index(position) == expected);
    }

    SECTION("Supports different spacing on each axis") {
        grid_type grid;
        grid.origin = {0.0f, 0.0f};
        grid.basis = {5.0f, 20.0f};

        const Vector<float, float> position{12.0f, 45.0f};

        const grid_type::Index expected{2, 2};

        REQUIRE(grid.get_index(position) == expected);
    }
}

TEST_CASE("Grid2 cardinal neighbors", "[Grid]") {
    SECTION("Returns four neighbors for an interior position") {
        Grid2 grid;
        grid.size = {10, 10};

        const Grid2::Index position{5, 5};

        const std::vector<Grid2::Index> expected{
            {4, 5}, {6, 5},
            {5, 4}, {5, 6}
        };

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_4(position);

        REQUIRE(actual.size() == expected.size());

        for (const Grid2::Index& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor)
                  != actual.end());
        }
    }

    SECTION("Respects grid boundaries") {
        Grid2 grid;
        grid.size = {10, 10};

        const Grid2::Index position{0, 0};

        const std::vector<Grid2::Index> expected{
            {1, 0}, {0, 1}
        };

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_4(position);

        REQUIRE(actual.size() == expected.size());

        for (const Grid2::Index& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor)
                  != actual.end());
        }
    }

    SECTION("Supports unbounded axes") {
        Grid2 grid;
        grid.size = {-1, 10};

        const Grid2::Index position{0, 5};

        const std::vector<Grid2::Index> expected{
            {-1, 5}, {1, 5},
            {0, 4}, {0, 6}
        };

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_4(position);

        REQUIRE(actual.size() == expected.size());

        for (const Grid2::Index& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor)
                  != actual.end());
        }
    }
}

TEST_CASE("Grid2 eight neighbors", "[Grid]") {
    SECTION("Returns eight neighbors for an interior position") {
        Grid2 grid;
        grid.size = {10, 10};

        const Grid2::Index position{5, 5};

        const std::vector<Grid2::Index> expected{
            {4, 4}, {4, 5}, {4, 6},
            {5, 4},          {5, 6},
            {6, 4}, {6, 5}, {6, 6}
        };

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_8(position);

        REQUIRE(actual.size() == expected.size());

        for (const Grid2::Index& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor)
                  != actual.end());
        }
    }

    SECTION("Returns three neighbors at a corner") {
        Grid2 grid;
        grid.size = {10, 10};

        const Grid2::Index position{0, 0};

        const std::vector<Grid2::Index> expected{
            {0, 1}, {1, 0}, {1, 1}
        };

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_8(position);

        REQUIRE(actual.size() == expected.size());

        for (const Grid2::Index& neighbor : expected) {
            CHECK(std::find(actual.begin(), actual.end(), neighbor)
                  != actual.end());
        }
    }

    SECTION("Does not include the original position") {
        Grid2 grid;
        grid.size = {10, 10};

        const Grid2::Index position{5, 5};

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_8(position);

        CHECK(std::find(actual.begin(), actual.end(), position)
              == actual.end());
    }

    SECTION("Supports unbounded grids") {
        Grid2 grid;
        grid.size = {-1, -1};

        const Grid2::Index position{0, 0};

        const std::vector<Grid2::Index> actual =
            grid.get_neighbors_8(position);

        REQUIRE(actual.size() == 8);
    }
}