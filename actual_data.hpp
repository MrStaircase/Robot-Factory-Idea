#pragma once

#include <entt/entt.hpp>

struct color{
    float r;
    float g;
    float b;
};

struct Point{
    float x;
    float y;
};

extern entt::registry registry;

void setup();

color get_color();

Point& get_point();

void add_color();

void set_point(float x, float y);