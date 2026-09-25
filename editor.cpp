#include "editor.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

bool Editor::init(SDL_Window* window, SDL_Renderer* _renderer){
    renderer = _renderer;

    IMGUI_CHECKVERSION();

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

    if (ImGui::Button("Add 16 Red"))
    {
        add_color();
    }

    ImGui::End();

    ImGui::Render();

    ImGui_ImplSDLRenderer3_RenderDrawData(
        ImGui::GetDrawData(),
        renderer
    );

}

void Editor::event(const SDL_Event& event){
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void Editor::quit(){
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();

    ImGui::DestroyContext();
}