#include "actual_data.hpp"

entt::registry registry;

void setup(){
    const auto entity = registry.create();
    registry.emplace<color>(entity, 200.0f, 0.0f, 0.0f);
}

color get_color(){
    auto view = registry.view<color>();
    return view.get<color>(view.front());
}

void add_color(){
    auto view = registry.view<color>();
    auto& c1 = view.get<color>(view.front());
    c1.r += 16.0f;
}