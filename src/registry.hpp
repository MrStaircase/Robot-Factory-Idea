#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include "data.hpp"
#include "save.hpp"

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

    void serialize(nlohmann::json& json) const {
        json = {
            {"point", {{"x", p.x}, {"y", p.y}}},
            {"radius", r},
            {"angle", angle}
        };
    }

    static AroundAPoint createInstance(const nlohmann::json& data) {
        return AroundAPoint{
            Point{
                data.at("point").at("x").get<float>(),
                data.at("point").at("y").get<float>()
            },
            data.at("radius").get<float>(),
            data.at("angle").get<float>()
        };
    }

    static inline const bool registered =
        DataManager::instance().registerType<AroundAPoint>("ArountAPoint");
};

struct DrawRect{
    Rect rect;
    color c;

    void serialize(nlohmann::json& json) const {
        json = {
            {"Rect", {{"x", rect.x}, {"y", rect.y}, {"h", rect.h}, {"w", rect.w}}},
            {"color", {{"r", c.r}, {"g", c.g}, {"b", c.b}}}
        };
    }

    static DrawRect createInstance(const nlohmann::json& data) {
        return DrawRect{
            Rect{
                data.at("Rect").at("x").get<float>(),
                data.at("Rect").at("y").get<float>(),
                data.at("Rect").at("h").get<float>(),
                data.at("Rect").at("w").get<float>()
            },
            color{
                data.at("color").at("r").get<float>(),
                data.at("color").at("g").get<float>(),
                data.at("color").at("b").get<float>()
            }
        };
    }

    static inline const bool registered =
        DataManager::instance().registerType<DrawRect>("DrawRect");
};

struct Selectable{
    void serialize(nlohmann::json& json) const {
        json = {};
    }

    static Selectable createInstance(const nlohmann::json& data) {
        return Selectable{};
    }

    static inline const bool registered =
        DataManager::instance().registerType<Selectable>("Selectable");
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

    const entt::registry& get_registry() const;
};