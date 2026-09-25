cmake_minimum_required(VERSION 3.16)
project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/entt EXCLUDE_FROM_ALL)

add_executable(main WIN32 main.cpp data.cpp actual_data.cpp game.cpp editor.cpp

    # Dear ImGui
    vendored/imgui/imgui.cpp
    vendored/imgui/imgui_draw.cpp
    vendored/imgui/imgui_tables.cpp
    vendored/imgui/imgui_widgets.cpp
    vendored/imgui/backends/imgui_impl_sdl3.cpp
    vendored/imgui/backends/imgui_impl_sdlrenderer3.cpp
    )

target_include_directories(main PRIVATE
    vendored/imgui
    vendored/imgui/backends
)

target_link_libraries(main PRIVATE SDL3::SDL3 EnTT::EnTT)