#pragma once

#include <entt/entt.hpp>

struct GameState{
    bool paused = false;
    bool next_frame = false;
};

struct color{
    float r;
    float g;
    float b;
};

struct Point{
    float x;
    float y;
};

struct Rect{
    float x;
    float y;
    float h;
    float w;
};

struct AroundAPoint{
    Point p;
    float r;
    float angle;
};

struct DrawRect{
    Rect rect;
    color c;
};


class Registry{
private:
    entt::registry registry;
public:
    void init();

    void destroy(const entt::entity&);
    bool valid(const entt::entity&) const;

    entt::entity create_new_entity();

    template<typename Component>
    Component& get(const entt::entity& entity){
        return registry.get<Component>(entity);
    }
    template<typename Component>
    const Component& get(const entt::entity& entity) const{
        return registry.get<Component>(entity);
    }

    template<typename Component>
    Component* try_get(const entt::entity& entity){
        return registry.try_get<Component>(entity);
    }
    template<typename Component>
    const Component* try_get(const entt::entity& entity) const{
        return registry.try_get<Component>(entity);
    }

    template<typename Component>
    void set(const entt::entity& entity, const Component& value){
        registry.emplace_or_replace<Component>(entity, value);
    }
    template<typename Component>
    void set(const entt::entity& entity, Component&& value){
        registry.emplace_or_replace<Component>(entity, std::move(value));
    }

    template<typename... Components>
    bool has_all(const entt::entity& entity) const{
        return registry.all_of<Components...>(entity);
    }

    template<typename... Components>
    auto view(){
        return registry.view<Components...>();
    }

    template<typename Component>
    void remove(const entt::entity& entity){
        registry.remove<Component>(entity);
    }

    GameState& game_state();
    const GameState& game_state() const;
};