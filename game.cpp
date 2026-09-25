#include "game.hpp"

static SDL_Point point;
static float radius = 100;

bool Game::init(SDL_Window** window, SDL_Renderer** _renderer){
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    if (!SDL_CreateWindowAndRenderer("Hello ECS", 1280, 720, 0, window, _renderer)) return false;

    renderer = *_renderer;

    point.x = 250;
    point.y = 150;

    return true;
}

void Game::iterate(){
    SDL_FRect rect;

    float angle = ((double)SDL_GetTicks()) / 1000.0;
    rect.w = 50;
    rect.h = 50;

    SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    color c1 = get_color();

    SDL_SetRenderDrawColor(renderer, c1.r, c1.g, c1.b, SDL_ALPHA_OPAQUE);
    rect.x = point.x + SDL_cos(angle) * radius;
    rect.y = point.y + SDL_sin(angle) * radius;
    SDL_RenderFillRect(renderer, &rect);

}

bool Game::event(const SDL_Event& event){
    if (event.type == SDL_EVENT_QUIT)
        return false;

    return true;
}

void Game::quit(){
    SDL_DestroyRenderer(renderer);
}