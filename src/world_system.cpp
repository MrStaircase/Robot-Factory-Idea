#include "world_system.hpp"

WorldSystem::WorldSystem(Registry& _registry) : registry(_registry) { }

WorldSystem::WorldSystem(Registry& _registry, const Grid2& _grid) :
    registry(_registry), grid(_grid) { }

WorldSystem::WorldSystem(Registry& _registry, Grid2&& _grid) :
    registry(_registry), grid(std::move(_grid)) { }

void WorldSystem::find_path(const entt::entity& entity, const Transform& t, FindPath& f){
    Pathfinder::WeightMap weight_map{};
    Pathfinder pathfinder{weight_map, grid};

    FollowPath path{pathfinder.a_star_cardinal(t.position, f.goal)};
    
    registry.remove<FindPath>(entity);
    registry.set<FollowPath>(entity, path);
}

void WorldSystem::init(){}

void WorldSystem::update(){
    for(const entt::entity& entity : registry.view<Transform, FindPath>()){
        const Transform& t = registry.get<Transform>(entity);
        FindPath& f = registry.get<FindPath>(entity);
        find_path(entity, t, f);
    }
}
