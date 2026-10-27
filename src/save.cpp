#include "save.hpp"


void save_registry(const entt::registry& registry, const std::string& filename){
    nlohmann::json data;
    data["entities"] = nlohmann::json::array();

    for (entt::entity entity: registry.view<entt::entity>()){
        nlohmann::json json;

        json["id"] = entity;

        for (const auto& [name, callbacks]: DataManager::instance().factories){
            callbacks.serializer(registry, entity, json);
        }
        
        data["entities"].emplace_back(json);
    }

    std::ofstream file(filename);

    file << std::setw(4) << data << std::endl;
}

entt::registry load_registry(const std::string& filename){
    std::ifstream file(filename);

    if (!file){
        return entt::registry();
    }

    nlohmann::json json;
    file >> json;

    entt::registry registry;

    for (const auto& entityData : json.at("entities")) {
        entt::entity entity = registry.create();

        for (auto it = entityData.begin(); it != entityData.end(); ++it) {
            if (it.key() == "id") {
                continue;
            }

            DataManager::instance().create(
                it.key(),
                registry,
                entity,
                it.value()
            );
        }
    }

    return registry;
}