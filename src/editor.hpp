#pragma once

#include <SDL3/SDL.h>
#include "registry.hpp"
#include <memory>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

class Editor{
public:
    Editor(Registry&);
    bool init(SDL_Window*, SDL_Renderer*);
    void iterate();
    void select_entity();
    void event(const SDL_Event&);
    void quit();

private:
    Registry& registry;
    entt::entity selected;
    SDL_Renderer* renderer;
};