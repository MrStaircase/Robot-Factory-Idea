#include "game.hpp"

static float radius = 100;

Game::Game(Registry& _registry): registry(_registry) {}

bool Game::init(SDL_Window** window, SDL_Renderer** _renderer){
    if (!SDL_CreateWindowAndRenderer("Hello ECS", 1280, 720, SDL_WINDOW_RESIZABLE, window, _renderer)){
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }

    renderer = *_renderer;
    last_frame = SDL_GetTicks();

    // this line makes sdl maintaine the aspect ratio, even if the window is rezised.
    // if used imgui does not work properlly
    // SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return true;
}

void Game::update(Uint64 delta){
    if (!registry.game_state().paused){
        angle += (double)delta / MILLISECOND_IN_SECOND;
    }
    else{
        if(registry.game_state().next_frame){
            registry.game_state().next_frame = false;
            angle += (double)FRAME_TIME / MILLISECOND_IN_SECOND;
        }
    }
}

void Game::iterate(){
    Uint64 current_frame = SDL_GetTicks();
    Uint64 delta = current_frame - last_frame;

    if (delta > FRAME_TIME){
        last_frame += FRAME_TIME;
        update(FRAME_TIME);
    }
    else{
        last_frame = current_frame;
        update(delta);
    }

    SDL_FRect rect;
    rect.w = 50;
    rect.h = 50;

    Point p = get_point();

    SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    color c1 = get_color();

    SDL_SetRenderDrawColor(renderer, c1.r, c1.g, c1.b, SDL_ALPHA_OPAQUE);
    rect.x = p.x + SDL_cos(angle) * radius - (rect.w / 2);
    rect.y = p.y + SDL_sin(angle) * radius - (rect.h / 2);
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