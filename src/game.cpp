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
    auto view = registry.view<AroundAPoint>();
    for (entt::entity entity : view){
        AroundAPoint& movement = registry.get<AroundAPoint>(entity);
        movement.angle += (double)delta / MILLISECOND_IN_SECOND;
    }
}

void Game::try_update(Uint64 delta){
    if (!registry.game_state().paused){
        update(delta);
    }
    else{
        if(registry.game_state().next_frame){
            registry.game_state().next_frame = false;
            update(FRAME_TIME);
        }
    }
}

void Game::iterate(){
    Uint64 current_frame = SDL_GetTicks();
    Uint64 delta = current_frame - last_frame;

    if (delta > FRAME_TIME){
        last_frame += FRAME_TIME;
        try_update(FRAME_TIME);
    }
    else{
        last_frame = current_frame;
        try_update(delta);
    }
    
    SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    
    auto view = registry.view<DrawRect, AroundAPoint>();
    for (entt::entity entity : view){
        DrawRect& draw_rect = registry.get<DrawRect>(entity);
        AroundAPoint& movement = registry.get<AroundAPoint>(entity);
        SDL_SetRenderDrawColor(renderer, draw_rect.c.r, draw_rect.c.g, draw_rect.c.b, SDL_ALPHA_OPAQUE);
        SDL_FRect rect;
        rect.w = draw_rect.rect.w;
        rect.h = draw_rect.rect.h;
        rect.x = movement.p.x + SDL_cos(movement.angle) * movement.r - (rect.w / 2);
        rect.y = movement.p.y + SDL_sin(movement.angle) * movement.r - (rect.h / 2);
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool Game::event(const SDL_Event& event){
    if (event.type == SDL_EVENT_QUIT)
        return false;

    return true;
}

void Game::quit(){
    SDL_DestroyRenderer(renderer);
}