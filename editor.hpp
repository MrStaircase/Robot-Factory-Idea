#pragma once

#include <SDL3/SDL.h>
#include "actual_data.hpp"
#include <memory>

class Editor{
public:
    bool init(SDL_Window* window, SDL_Renderer* renderer);
    void iterate();
    void event(const SDL_Event& event);
    void quit();
};