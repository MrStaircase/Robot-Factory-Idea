#include "world_system.hpp"

WorldSystem::WorldSystem(Registry& _registry) : registry(_registry) { }

WorldSystem::WorldSystem(Registry& _registry, const Grid2& _grid) :
    registry(_registry), grid(_grid) { }

WorldSystem::WorldSystem(Registry& _registry, Grid2&& _grid) :
    registry(_registry), grid(std::move(_grid)) { }

void something(Grid2 grid, const Transform& t, const FindPath& f){
    WorldSystem::find_path(t.position, f.goal);
}

void WorldSystem::find_path(const Vector2I& start, const Vector2I& goal){

}

void WorldSystem::init(){}

void WorldSystem::update(){
    auto view = registry.view<Transform, FindPath>();
    view.each(std::bind(something, grid, std::placeholders::_1, std::placeholders::_2));
}
