#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

template<typename T>
concept HasJsonKey = requires {
    { T::json_key } -> std::convertible_to<std::string>;
};

template<typename T>
concept SerializableComponent =
    HasJsonKey<T> &&
    (
        (std::is_empty_v<T> && std::default_initializable<T>)
        ||
        requires(
            const T& const_component,
            T& mutable_component,
            nlohmann::json& mutable_json,
            const nlohmann::json& const_json
        ) {
            { to_json(mutable_json, const_component) } -> std::same_as<void>;
            { from_json(const_json, mutable_component) } -> std::same_as<void>;
        }
    );

class DataManager {;
private:
    using Serializer = std::function<void(
        const entt::registry&,
        entt::entity,
        nlohmann::json&
    )>;

    using Deserializer = std::function<void(
        entt::registry&,
        entt::entity,
        const nlohmann::json&
    )>;

    using HasComponent = std::function<bool(
        const entt::registry&,
        entt::entity
    )>;

    struct ComponentInfo {
        Serializer serialize;
        Deserializer deserialize;
        HasComponent has;
    };

    std::unordered_map<std::string, ComponentInfo> components;

public:
    DataManager() = default;

    template<SerializableComponent T>
    bool register_component(const std::string& name) {
        if (components.contains(name)) {
            throw std::runtime_error(
                "Component already registered: " + name
            );
        }

        ComponentInfo info;

        info.has = [](const entt::registry& registry,
                      entt::entity entity) {
            return registry.all_of<T>(entity);
        };

        info.serialize = [](const entt::registry& registry,
                            entt::entity entity,
                            nlohmann::json& json) {
            if constexpr (std::is_empty_v<T>) {
                json = nlohmann::json::object();
            } else {
                json = registry.get<T>(entity);
            }
        };

        info.deserialize = [](entt::registry& registry,
                              entt::entity entity,
                              const nlohmann::json& json) {
            if constexpr (std::is_empty_v<T>) {
                registry.emplace_or_replace<T>(entity);
            } else {
                registry.emplace_or_replace<T>(
                    entity,
                    json.get<T>()
                );
            }
        };

        components.emplace(name, std::move(info));
        return true;
    }

    bool deregister_component(const std::string& name){
        if (components.contains(name)) {
            return components.erase(name) != 0;
        }
        return false;
    }

    nlohmann::json serialize_entity(
        const entt::registry& registry,
        entt::entity entity
    ) const {
        nlohmann::json result = nlohmann::json::object();

        for (std::unordered_map<std::string, ComponentInfo>::const_iterator it = components.cbegin();
            it != components.cend();
            ++it) {
            const std::string& name = it->first;
            const ComponentInfo& info = it->second;

            if (!info.has(registry, entity)) {
                continue;
            }

            info.serialize(registry, entity, result[name]);
        }

        return result;
    }

    void deserialize_entity(
        entt::registry& registry,
        entt::entity entity,
        const nlohmann::json& json
    ) const {
        if (!json.is_object()) {
            throw std::runtime_error(
                "Entity component data must be a JSON object"
            );
        }

        for (nlohmann::json::const_iterator it = json.begin(); it != json.end(); ++it) {
            std::unordered_map<std::string, ComponentInfo>::const_iterator component_it = components.find(it.key());

            if (component_it == components.end()) {
                throw std::runtime_error(
                    "Unknown component type: " + it.key()
                );
            }

            component_it->second.deserialize(
                registry,
                entity,
                it.value()
            );
        }
    }

    bool is_registered(const std::string& name) const {
        return components.contains(name);
    }

    std::size_t registered_count() const {
        return components.size();
    }
};
