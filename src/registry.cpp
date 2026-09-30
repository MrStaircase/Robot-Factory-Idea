#include "registry.hpp"

#pragma region Registry::
void Registry::init(){
    registry.ctx().emplace<GameState>();
    // read from file
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

template<typename Component>
Component& Registry::get(const entt::entity& entity){
    return registry.get<Component>(entity);
}

template<typename Component>
const Component& Registry::get(const entt::entity& entity) const {
    return registry.get<Component>(entity);
}

template<typename Component>
Component* Registry::try_get(const entt::entity& entity){
    return registry.try_get<Component>(entity);
}

template<typename Component>
const Component* Registry::try_get(const entt::entity& entity) const {
    return registry.try_get<Component>(entity);
}

template<typename Component>
void Registry::set(const entt::entity& entity, const Component& value){
    registry.emplace_or_replace<Component>(entity, value);
}

template<typename Component>
void Registry::set(const entt::entity& entity, Component&& value){
    registry.emplace_or_replace<Component>(entity, std::move(value));
}

template<typename... Components>
bool Registry::has_all(const entt::entity& entity) const {
    return registry.all_of<Components...>(entity);
}

template<typename Component>
void Registry::remove(const entt::entity& entity){
    registry.remove<Component>(entity);
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