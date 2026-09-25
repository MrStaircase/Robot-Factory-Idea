#pragma once

#include "data.hpp"
#include <entt/entt.hpp>

struct color{
    float r;
    float g;
    float b;
};

extern entt::registry registry;

void setup();

color get_color();

void add_color();

extern Color playerColor;