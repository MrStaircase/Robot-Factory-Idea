#include "editor.hpp"

Editor::Editor(Registry& _registry): registry(_registry) {}

bool Editor::init(SDL_Window* window, SDL_Renderer* _renderer){
    renderer = _renderer;
    selected = entt::null;

    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer))
        return false;

    if (!ImGui_ImplSDLRenderer3_Init(renderer))
        return false;

    return true;
}

void Editor::iterate(){
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Editor");

    if (ImGui::Button("Add 16 Red")){
        auto view = registry.view<DrawRect>();
        for (entt::entity entity : view){
            DrawRect& draw_rect = registry.get<DrawRect>(entity);
            draw_rect.c.r += 16;
        }
    }

    if (ImGui::Button("Pause")){
        registry.game_state().paused = !registry.game_state().paused;
    }

    if (ImGui::Button("Next Frame")){
        registry.game_state().next_frame = true;
    }

    if (selected != entt::null){
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        AroundAPoint& movement = registry.get<AroundAPoint>(selected);
        ImGui::SliderFloat("X", &movement.p.x, 0, 800);
        ImGui::SliderFloat("Y", &movement.p.y, 0, 600);
        SDL_FRect rect;
        rect.h = rect.w = 10;
        rect.x = movement.p.x - (rect.w / 2);
        rect.y = movement.p.y - (rect.h / 2);
        SDL_RenderRect(renderer, &rect);
    }

    ImGui::End();

    ImGui::Render();

    ImGui_ImplSDLRenderer3_RenderDrawData(
        ImGui::GetDrawData(),
        renderer
    );

}

void Editor::select_entity(){
    float x, y;
    SDL_GetMouseState(&x, &y);
    auto view = registry.view<Selectable, DrawRect>();
    for (entt::entity entity: view){
        DrawRect& draw_rect = registry.get<DrawRect>(entity);
        if (x >= draw_rect.rect.x && x <= draw_rect.rect.x + draw_rect.rect.w
         && y >= draw_rect.rect.y && y <= draw_rect.rect.y + draw_rect.rect.h){
            selected = entity;
            return;
        }
    }
    selected = entt::null;
}

void Editor::event(const SDL_Event& event){
    ImGui_ImplSDL3_ProcessEvent(&event);

    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
        return;
    
    switch(event.type){
        case SDL_EVENT_MOUSE_BUTTON_DOWN: select_entity(); break;
    }
}

void Editor::quit(){
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();

    ImGui::DestroyContext();
}