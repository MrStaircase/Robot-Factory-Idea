#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include "data.hpp"
#include "save.hpp"

struct GameState{
    bool paused = false;
    bool next_frame = false;
};

struct Color{
    float r, g, b;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Color, r, g, b);
};


struct Point{
    float x, y;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Point, x, y);
};

struct Rect{
    float x, y, h, w;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rect, x, y, h, w);
};

struct AroundAPoint{
    static constexpr std::string json_key = "AroundAPoint";

    Point p;
    float r;
    float angle;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AroundAPoint, p, r, angle);
};

struct DrawRect{
    static constexpr std::string json_key = "DrawRect";

    Rect rect;
    Color c;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DrawRect, rect, c);
};

struct Selectable{
    static constexpr std::string json_key = "Selectable";
};

template<bool AutomaticRegistration>
class AutomaticRegistry{
private:
    entt::registry registry;
    DataManager data_manager;
public:
    void init(){
        registry.ctx().emplace<GameState>();
        
        entt::entity e = registry.create();
        registry.emplace<DrawRect>(e, Rect{0, 0, 50, 50}, Color{0, 0, 0});
        registry.emplace<AroundAPoint>(e, Point{500, 300}, 100.f, 0.f);
        registry.emplace<Selectable>(e);

        e = registry.create();
        registry.emplace<DrawRect>(e, Rect{0, 0, 150, 50}, Color{0, 0, 255});
        registry.emplace<AroundAPoint>(e, Point{200, 500}, 100.f, 1.5f);
        registry.emplace<Selectable>(e);

        data_manager.register_component<DrawRect>(DrawRect::json_key);
        data_manager.register_component<AroundAPoint>(AroundAPoint::json_key);
        data_manager.register_component<Selectable>(Selectable::json_key);
    }

    void destroy(const entt::entity& entity){
        registry.destroy(entity);
    }

    bool valid(const entt::entity& entity) const{
        return registry.valid(entity);
    }

    entt::entity create_new_entity(){
        return registry.create();
    }

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

        if constexpr (AutomaticRegistration && SerializableComponent<Component>){
            if (!data_manager.is_registered(Component::json_key)){
                data_manager.register_component<Component>(Component::json_key);
            }
        }
    }
    template<typename Component>
    void set(const entt::entity& entity, Component&& value){
        registry.emplace_or_replace<Component>(entity, std::move(value));

        if constexpr (AutomaticRegistration && SerializableComponent<Component>){
            if (!data_manager.is_registered(Component::json_key)){
                data_manager.register_component<Component>(Component::json_key);
            }
        }
    }

    template<SerializableComponent Component>
    bool register_component(const std::string& json_key){
        return data_manager.register_component<Component>(json_key);
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
        if (!registry.all_of<Component>(entity)) {
            return;
        }

        registry.remove<Component>(entity);

        if constexpr (AutomaticRegistration &&
                  SerializableComponent<Component>) {
            if (registry.storage<Component>()->empty()) {
                data_manager.deregister_component(Component::json_key);
            }
        }
    }

    void save(const std::string& filename) const{
        nlohmann::json save_data;
        save_data["entities"] = nlohmann::json::array();

        for (entt::entity entity : registry.view<entt::entity>()) {
            nlohmann::json entity_data =
                data_manager.serialize_entity(registry, entity);

            save_data["entities"].push_back(entity_data);
        }

        std::ofstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not open save file for writing: " + filename
            );
        }

        file << save_data.dump(4);

        if (!file) {
            throw std::runtime_error(
                "Failed to write save file: " + filename
            );
        }
    }
    void load(const std::string& filename){
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not open save file for reading: " + filename
            );
        }

        nlohmann::json save_data;
        file >> save_data;

        if (!save_data.is_object() ||
            !save_data.contains("entities") ||
            !save_data["entities"].is_array()) {
            throw std::runtime_error(
                "Invalid save file format: " + filename
            );
        }

        registry.clear();

        for (const nlohmann::json& entity_data : save_data["entities"]) {
            entt::entity entity = registry.create();

            data_manager.deserialize_entity(
                registry,
                entity,
                entity_data
            );
        }

        registry.ctx().emplace<GameState>();
    }

    GameState& game_state(){
        return registry.ctx().get<GameState>();
    }
    const GameState& game_state() const{
        return registry.ctx().get<GameState>();
    }

    const entt::registry& get_registry() const{
        return registry;
    }
};

using Registry = AutomaticRegistry<true>;