#include "save.hpp"

using nlohmann::json;

void save_registry(const entt::registry& registry, const std::string& filename){
    json data;
    data["entities"] = json::array();

    std::ofstream file(filename);

    file << std::setw(4) << data << std::endl;
}

entt::registry load_registry(const std::string& filename){
    return entt::registry();
}