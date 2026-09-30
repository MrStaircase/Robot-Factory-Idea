#define SDL_MAIN_USE_CALLBACKS /* use the callbacks instead of main() */

#include <SDL3/SDL_main.h>
#include "imgui.h"
#include "main.hpp"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Example Renderer Primitives", "1.0", "com.example.renderer-primitives");

    IMGUI_CHECKVERSION();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    setup();

    if (!game.init(&window, &renderer))
        return SDL_APP_FAILURE;

    if (!editor.init(window, renderer))
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    editor.event(*event);

    return game.event(*event)
        ? SDL_APP_CONTINUE
        : SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    game.iterate();
    editor.iterate();

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    game.quit();
    editor.quit();

    SDL_Quit();
}