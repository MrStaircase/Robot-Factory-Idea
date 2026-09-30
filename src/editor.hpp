#pragma once

#include <SDL3/SDL.h>
#include "registry.hpp"
#include <memory>

class Editor{
public:
    bool init(SDL_Window* window, SDL_Renderer* _renderer);
    void iterate();
    void event(const SDL_Event& event);
    void quit();

private:
    SDL_Renderer* renderer;
};