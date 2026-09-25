#include "editor.hpp"

bool Editor::init(SDL_Window* window, SDL_Renderer* renderer){
    // IMGUI_CHECKVERSION();

    // ImGui::CreateContext();

    // ImGui::StyleColorsDark();

    // if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer))
    //     return false;

    // if (!ImGui_ImplSDLRenderer3_Init(renderer))
    //     return false;

    return true;
}

void Editor::iterate(){
    // ImGui_ImplSDLRenderer3_NewFrame();
    // ImGui_ImplSDL3_NewFrame();
    // ImGui::NewFrame();

    // ImGui::Begin("Editor");

    // if (ImGui::Button("Add 16 Red"))
    // {
    //     // We'll fill this in once we define the ECS components.
    // }

    // ImGui::End();

    // ImGui::Render();

    // ImGui_ImplSDLRenderer3_RenderDrawData(
    //     ImGui::GetDrawData(),
    //     nullptr
    // );

}

void Editor::event(const SDL_Event& event){
    // ImGui_ImplSDL3_ProcessEvent(&event);
}

void Editor::quit(){
    // ImGui_ImplSDLRenderer3_Shutdown();
    // ImGui_ImplSDL3_Shutdown();

    // ImGui::DestroyContext();
}