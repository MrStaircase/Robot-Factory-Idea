#include <nlohmann/json.hpp>
#include "vector.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector2, x, y);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector2I, x, y);
