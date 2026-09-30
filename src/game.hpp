#pragma once

#include <SDL3/SDL.h>
#include "registry.hpp"
#include <memory>

constexpr int MILLISECOND_IN_SECOND = 1000;
constexpr int FRAME_RATE_PER_SECOND = 60;
constexpr int FRAME_TIME = MILLISECOND_IN_SECOND / FRAME_RATE_PER_SECOND;

class Game{
public:
    Game(Registry&);
    bool init(SDL_Window**, SDL_Renderer**);
    void update(Uint64);
    void try_update(Uint64);
    void iterate();
    bool event(const SDL_Event&);
    void quit();

private:
    Registry& registry;
    SDL_Renderer* renderer;
    Uint64 last_frame;
    float angle;
};