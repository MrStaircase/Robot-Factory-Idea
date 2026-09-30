# pragma once

#include <SDL3/SDL.h>
#include "game.hpp"
#include "editor.hpp"

SDL_Window* window;
SDL_Renderer* renderer;

Game game;
Editor editor;
