#pragma once

#include "registry.hpp"
#include "grid.hpp"
#include "path_find.hpp"
#include <functional>

class System{
public:
    virtual ~System() = default;
    virtual void init() = 0;
    virtual void update() = 0;
};

struct Solid{};

struct Transform{
    Vector2I position;
};

struct FindPath{
    Vector2I goal;
};

struct FollowPath{
    std::vector<Vector2I> path;
};

class WorldSystem : public System{
private:
public:
    void find_path(const entt::entity&, const Transform&, FindPath&);
    using find_path_function = std::function<void(const entt::entity&, const Transform&, FindPath&)>;
    
    Registry& registry;
    Grid2 grid;

    WorldSystem() = delete;

    WorldSystem(Registry&);
    WorldSystem(Registry&, const Grid2&);
    WorldSystem(Registry&, Grid2&&);

    void init() override;
    void update() override;
};