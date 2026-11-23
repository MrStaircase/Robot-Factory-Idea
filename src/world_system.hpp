#pragma once

#include "registry.hpp"
#include "grid.hpp"

class System{
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

class WorldSystem : public System{
private:
public:
    static void find_path(const Vector2I&, const Vector2I&);
    Registry& registry;
    Grid2 grid;

    WorldSystem() = delete;

    WorldSystem(Registry&);
    WorldSystem(Registry&, const Grid2&);
    WorldSystem(Registry&, Grid2&&);

    void init() override;
    void update() override;
};