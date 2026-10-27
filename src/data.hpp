#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
template<std::size_t N>
struct FixedString {
    char value[N];

    constexpr FixedString(char const (&s)[N]) {
        for (std::size_t i = 0; i < N; ++i)
            value[i] = s[i];
    }

    constexpr std::string_view view() const {
        return {value, N - 1};
    }
};

template<typename T, FixedString Name>
class Property {
public:
    static constexpr auto json_name = Name;

    T value{};

    constexpr std::string get_name() const {
        return std::string(json_name.view());
    }
};

using PositionX = Property<int, "position_x">;
using PositionY = Property<int, "position_y">;

template<typename T>
concept SerializeableComponent = 
    std::is_empty_v<T> ||
    requires(const T& component, nlohmann::json& json,
        const nlohmann::json& data){
        {component.serialize(json)} -> std::same_as<void>;
        {T::createInstance(data)} -> std::same_as<T>;
    };

class DataManager {
    using Serializer = std::function<void(
        const entt::registry&,
        entt::entity,
        nlohmann::json&
    )>;

    using Factory = std::function<void(
        entt::registry&,
        entt::entity,
        const nlohmann::json&
    )>;

    struct Something{
        Serializer serializer;
        Factory factory;
    };

    
public:
    std::unordered_map<std::string, Something> factories;
    static DataManager& instance() {
        static DataManager manager;
        return manager;
    }

    template<SerializeableComponent T>
    bool registerType(const std::string& name) {
        factories[name].serializer = [name](const entt::registry& registry,
                             entt::entity entity,
                             nlohmann::json& json) {
            if (!registry.all_of<T>(entity)) {
                return;
            }

            if constexpr (std::is_empty_v<T>) {
                json[name] = nlohmann::json::object();
            } else {
                registry.get<T>(entity).serialize(json[name]);
            }
        };

        factories[name].factory = [](entt::registry& registry,
                             entt::entity entity,
                             const nlohmann::json& data) {
            if constexpr (std::is_empty_v<T>) {
                registry.emplace<T>(entity);
            } else {
                registry.emplace<T>(
                    entity,
                    T::createInstance(data)
                );
            }
        };

        return true;
    }

    void create(
        const std::string& name,
        entt::registry& registry,
        entt::entity entity,
        const nlohmann::json& data
    ) const {
        auto it = factories.find(name);

        if (it == factories.end())
            throw std::runtime_error("Unknown component: " + name);

        it->second.factory(registry, entity, data);
    }
};

#define REGISTER_COMPONENT(Type, Name) \
    namespace { \
        inline const bool Type##_registered = \
            DataManager::instance().registerType<Type>(Name); \
    }

struct DataTypeA{
    std::string point;
    float radius;
    float angle;

    void serialize(nlohmann::json& json) const {
        json = {
            {"point", point},
            {"radius", radius},
            {"angle", angle}
        };
    }

    static DataTypeA createInstance(const nlohmann::json& data) {
        return DataTypeA{
            data.at("point").get<std::string>(),
            data.at("radius").get<float>(),
            data.at("angle").get<float>()
        };
    }
};

REGISTER_COMPONENT(DataTypeA, "DataTypeA")
