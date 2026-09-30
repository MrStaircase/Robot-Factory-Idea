#include "actual_data.hpp"

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