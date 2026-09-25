#pragma once

#include <SDL3/SDL.h>
#include "actual_data.hpp"
#include <memory>

class Game{
public:
    bool init(SDL_Window** window, SDL_Renderer** _renderer);
    void iterate();
    bool event(const SDL_Event& event);
    void quit();

private:
    SDL_Renderer* renderer;
};