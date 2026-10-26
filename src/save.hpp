#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <fstream>

void save_registry(const entt::registry& registry, const std::string& filename);
entt::registry load_registry(const std::string& filename);
