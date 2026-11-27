#pragma once

#include "registry.hpp"
#include <SDL3/SDL.h>
#include <functional>

struct Drawable{
    std::function<void(SDL_Renderer&)> draw;
};

