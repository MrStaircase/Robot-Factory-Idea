#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

template<typename T>
concept HasJsonKey = requires {
    { T::json_key } -> std::convertible_to<std::string>;
};

template<typename T>
concept JsonConvertible = requires(
    const T& const_component,
    T& mutable_component,
    nlohmann::json& mutable_json,
    const nlohmann::json& const_json
) {
    { to_json(mutable_json, const_component) } -> std::same_as<void>;
    { from_json(const_json, mutable_component) } -> std::same_as<void>;
};

template<typename T>
concept SerializeableComponent = HasJsonKey<T> && JsonConvertible<T>;

template<typename T>
concept EmptyComponent =
    HasJsonKey<T> &&
    std::is_empty_v<T> &&
    std::default_initializable<T>;

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

    struct ComponentFactory {
        Serializer serialize;
        Deserializer deserialize;
        HasComponent has;
    };

    std::unordered_map<std::string, ComponentFactory> factories;

    using FactoryCIterator = std::unordered_map<std::string, ComponentFactory>::const_iterator;

public:
    static DataManager& instance(){
        static DataManager manager;
        return manager;
    }

    template<SerializeableComponent T>
    bool register_component(){
        ComponentFactory factory;
        
        factory.serialize = [](
            const entt::registry& registry,
            const entt::entity& entity,
            nlohmann::json& json
        ){
            json =  registry.get<T>(entity);
        };

        factory.deserialize = [](
            entt::registry& registry,
            const entt::entity& entity,
            const nlohmann::json& json
        ){
            registry.emplace_or_replace<T>(entity, json.get<T>());
        };

        factory.has = [](
            const entt::registry& registry,
            const entt::entity& entity
        ){
            return registry.all_of<T>(entity);
        };

        factories[T::json_key] = factory;

        return true;
    }

    template<EmptyComponent T>
    bool register_component(){
        ComponentFactory factory;

        factory.serialize = [](
            const entt::registry& registry,
            const entt::entity& entity,
            nlohmann::json& json
        ){
            json = nlohmann::json::object();
        };

        factory.deserialize = [](
            entt::registry& registry,
            const entt::entity& entity,
            const nlohmann::json& json
        ){
            registry.emplace_or_replace<T>(entity);
        };

        factory.has = [](
            const entt::registry& registry,
            const entt::entity& entity
        ){
            return registry.all_of<T>(entity);
        };

        factories[T::json_key] = factory;
        
        return true;
    }

    nlohmann::json serialize_entity(
        const entt::registry& registry,
        const entt::entity& entity
    ) const {
        nlohmann::json result = nlohmann::json::object();

        for (
            FactoryCIterator it = factories.cbegin();
            it != factories.cend();
            ++it
        ) {
            const std::string& json_key = it->first;
            const ComponentFactory& factory = it->second;

            if (factory.has(registry, entity)) {
                factory.serialize(registry, entity, result[json_key]);
            }
        }

        return result;
    }

    void deserialize_entity(
        entt::registry& registry,
        const entt::entity& entity,
        const nlohmann::json& json
    ) const {
        if (!json.is_object()) {
            throw std::runtime_error(
                "deserialize_entity: Entity component data must be a JSON object"
            );
        }

        for (
            nlohmann::json::const_iterator it = json.begin();
            it != json.end();
            ++it
        ) {
            FactoryCIterator factory_it = factories.find(it.key());

            if (factory_it == factories.end()) {
                throw std::runtime_error(
                    "Unknown component type: " + it.key()
                );
            }

            factory_it->second.deserialize(
                registry,
                entity,
                it.value()
            );
        }
    }

    bool is_registered(const std::string& json_key) const {
        return factories.contains(json_key);
    }

    template<EmptyComponent T>
    bool is_registred() const {
        return factories.contains(T::json_key);
    }

    std::size_t registered_count() const {
        return factories.size();
    }
};

#define REGISTER_COMPONENT(Type) \
    namespace { \
        inline const bool Type##_registered = \
            DataManager::instance().register_component<Type>(); \
    }
