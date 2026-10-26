#include "registry.hpp"

#pragma region Registry::
void Registry::init(){
    registry.ctx().emplace<GameState>();
    // TODO: read from file

    entt::entity e = registry.create();
    registry.emplace<DrawRect>(e, Rect{0, 0, 50, 50}, color{0, 0, 0});
    registry.emplace<AroundAPoint>(e, Point{500, 300}, 100.f, 0.f);
    registry.emplace<Selectable>(e);

    e = registry.create();
    registry.emplace<DrawRect>(e, Rect{0, 0, 150, 50}, color{0, 0, 255});
    registry.emplace<AroundAPoint>(e, Point{200, 500}, 100.f, 1.5f);
    registry.emplace<Selectable>(e);
}

void Registry::destroy(const entt::entity& entity){
    registry.destroy(entity);
}

bool Registry::valid(const entt::entity& entity) const{
    return registry.valid(entity);
}

entt::entity Registry::create_new_entity(){
    return registry.create();
}

GameState& Registry::game_state(){
    return registry.ctx().get<GameState>();
}

const GameState& Registry::game_state() const {
    return registry.ctx().get<GameState>();
}

Registry registry1;

#pragma endregion

entt::registry registry;


void setup(){
    const auto entity = registry.create();
    registry.emplace<color>(entity, 200.0f, 0.0f, 0.0f);
    registry.emplace<Point>(entity, 500.f, 300.f);
}

color get_color(){
    auto view = registry.view<color>();
    return view.get<color>(view.front());
}

Point& get_point(){
    auto view = registry.view<Point>();
    return view.get<Point>(view.front());
}

void add_color(){
    auto view = registry.view<color>();
    color& c1 = view.get<color>(view.front());
    c1.r += 16.0f;
}

void set_point(float x, float y){
    auto view = registry.view<Point>();
    Point& p = view.get<Point>(view.front());
    p.x = x;
    p.y = y;
}