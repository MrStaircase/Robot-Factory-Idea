# pragma once

#include <SDL3/SDL.h>
#include "game.hpp"
#include "editor.hpp"
#include "registry.hpp"

class MainClass{
public:
    MainClass(): game(registry), editor(registry){}

    SDL_Window* window;
    SDL_Renderer* renderer;
    
    Registry registry;
    Game game;
    Editor editor;

};

MainClass mainclass;
