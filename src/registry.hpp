#pragma once

#include <entt/entt.hpp>

struct GameState{
    bool paused = false;
    bool next_frame = false;
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
    Component& get(const entt::entity&);
    template<typename Component>
    const Component& get(const entt::entity&) const;

    template<typename Component>
    Component* try_get(const entt::entity&);
    template<typename Component>
    const Component* try_get(const entt::entity&) const;

    template<typename Component>
    void set(const entt::entity&, const Component&);
    template<typename Component>
    void set(const entt::entity&, Component&&);

    template<typename... Components>
    bool has_all(const entt::entity&) const;

    template<typename Component>
    void remove(const entt::entity&);

    GameState& game_state();
    const GameState& game_state() const;
};

extern Registry registry1;

struct color{
    float r;
    float g;
    float b;
};

struct Point{
    float x;
    float y;
};

extern entt::registry registry;

void setup();

color get_color();

Point& get_point();

void add_color();

void set_point(float x, float y);