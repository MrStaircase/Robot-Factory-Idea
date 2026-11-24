#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <cstdlib>
#include <ostream>
#include <vector>

#include "world_system.hpp"

namespace {

    Grid2 make_open_grid(){
        Grid2 g;
        g.size = {10, 10};
        return Grid2{g};
    }

    entt::entity make_path_request(Registry& registry, const Vector2I& start, const Vector2I& goal){
        const entt::entity entity = registry.create_new_entity();
        registry.set<Transform>(entity, Transform{start});
        registry.set<FindPath>(entity, FindPath{goal});
        return entity;
    }

    bool is_cardinal_step(const Vector2I& a, const Vector2I& b){
        return std::abs(a.x - b.x) + std::abs(a.y - b.y) == 1;
    }

}

TEST_CASE("WorldSystem resolves a FindPath request", "[world_system][pathfinding]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    const Vector2I start{0, 0};
    const Vector2I goal{3, 2};
    const entt::entity entity = make_path_request(registry, start, goal);

    world.update();

    SECTION("the request is consumed"){
        CHECK_FALSE(registry.all_of<FindPath>(entity));
    }

    SECTION("a FollowPath is attached"){
        REQUIRE(registry.all_of<FollowPath>(entity));
    }

    SECTION("the path ends at the goal"){
        REQUIRE(registry.all_of<FollowPath>(entity));
        const FollowPath& follow = registry.get<FollowPath>(entity);
        REQUIRE_FALSE(follow.path.empty());
        CHECK(follow.path.front() == goal);
    }

    SECTION("the path only moves in cardinal steps"){
        REQUIRE(registry.all_of<FollowPath>(entity));
        const FollowPath& follow = registry.get<FollowPath>(entity);
        REQUIRE_FALSE(follow.path.empty());
        for(std::size_t i = 1; i < follow.path.size(); ++i){
            INFO("step " << i << ": " << follow.path[i - 1] << " -> " << follow.path[i]);
            CHECK(is_cardinal_step(follow.path[i - 1], follow.path[i]));
        }
    }

    SECTION("the Transform is not modified by pathfinding"){
        const Transform& t = registry.get<Transform>(entity);
        CHECK(t.position == start);
    }
}

TEST_CASE("WorldSystem path length is optimal on an open grid", "[world_system][pathfinding]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    const Vector2I start{1, 1};
    const Vector2I goal{6, 4};
    const entt::entity entity = make_path_request(registry, start, goal);

    world.update();

    REQUIRE(registry.all_of<FollowPath>(entity));
    const FollowPath& follow = registry.get<FollowPath>(entity);

    const std::size_t manhattan = static_cast<std::size_t>(
        std::abs(goal.x - start.x) + std::abs(goal.y - start.y));
    CHECK(follow.path.size() == manhattan + 1);
}

TEST_CASE("WorldSystem handles several requests in one update", "[world_system]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    const Vector2I goal_a{4, 0};
    const Vector2I goal_b{0, 5};
    const Vector2I goal_c{7, 7};
    const entt::entity a = make_path_request(registry, Vector2I{0, 0}, goal_a);
    const entt::entity b = make_path_request(registry, Vector2I{0, 0}, goal_b);
    const entt::entity c = make_path_request(registry, Vector2I{2, 2}, goal_c);

    world.update();

    REQUIRE(registry.all_of<FollowPath>(a));
    REQUIRE(registry.all_of<FollowPath>(b));
    REQUIRE(registry.all_of<FollowPath>(c));

    CHECK_FALSE(registry.all_of<FindPath>(a));
    CHECK_FALSE(registry.all_of<FindPath>(b));
    CHECK_FALSE(registry.all_of<FindPath>(c));

    CHECK(registry.get<FollowPath>(a).path.front() == goal_a);
    CHECK(registry.get<FollowPath>(b).path.front() == goal_b);
    CHECK(registry.get<FollowPath>(c).path.front() == goal_c);
}

TEST_CASE("WorldSystem ignores entities without a request", "[world_system]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    SECTION("an entity with only a Transform is untouched"){
        const entt::entity entity = registry.create();
        registry.emplace<Transform>(entity, Transform{Vector2I{2, 2}});

        world.update();

        CHECK_FALSE(registry.all_of<FollowPath>(entity));
        CHECK_FALSE(registry.all_of<FindPath>(entity));
    }

    SECTION("an entity with FindPath but no Transform is skipped"){
        const entt::entity entity = registry.create();
        registry.emplace<FindPath>(entity, FindPath{Vector2I{3, 3}});

        world.update();

        CHECK(registry.all_of<FindPath>(entity));
        CHECK_FALSE(registry.all_of<FollowPath>(entity));
    }

    SECTION("an empty registry does not crash"){
        Registry registry2;
        WorldSystem world2{registry2, make_open_grid()};
        world2.init();

        CHECK_NOTHROW(world2.update());
    }
}

TEST_CASE("WorldSystem replaces an existing FollowPath", "[world_system]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    const Vector2I goal{5, 5};
    const entt::entity entity = make_path_request(registry, Vector2I{0, 0}, goal);

    FollowPath stale;
    stale.path.push_back(Vector2I{9, 9});
    registry.emplace<FollowPath>(entity, stale);

    world.update();

    REQUIRE(registry.all_of<FollowPath>(entity));
    const FollowPath& follow = registry.get<FollowPath>(entity);
    REQUIRE_FALSE(follow.path.empty());
    CHECK(follow.path.front() == goal);
    CHECK(follow.path.size() > 1);
}

TEST_CASE("WorldSystem does not re-run a consumed request", "[world_system]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    const entt::entity entity = make_path_request(registry, Vector2I{0, 0}, Vector2I{4, 4});

    world.update();
    REQUIRE(registry.all_of<FollowPath>(entity));
    const std::vector<Vector2I> first_path = registry.get<FollowPath>(entity).path;

    registry.get<FollowPath>(entity).path.clear();
    world.update();

    CHECK(registry.get<FollowPath>(entity).path.empty());
    CHECK_FALSE(first_path.empty());
}

TEST_CASE("WorldSystem edge cases for start and goal", "[world_system][pathfinding]"){
    Registry registry;
    WorldSystem world{registry, make_open_grid()};
    world.init();

    SECTION("start equals goal"){
        const Vector2I position{3, 3};
        const entt::entity entity = make_path_request(registry, position, position);

        world.update();

        CHECK_FALSE(registry.all_of<FindPath>(entity));
        REQUIRE(registry.all_of<FollowPath>(entity));
        CHECK(registry.get<FollowPath>(entity).path.size() <= 1);
    }

    SECTION("goal outside the grid"){
        const entt::entity entity = make_path_request(registry, Vector2I{0, 0}, Vector2I{100, 100});

        CHECK_THROWS_AS(world.update(), std::out_of_range);
        CHECK(registry.all_of<FindPath>(entity));
        CHECK_FALSE(registry.all_of<FollowPath>(entity));
    }
}