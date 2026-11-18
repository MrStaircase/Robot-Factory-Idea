cmake_minimum_required(VERSION 3.20)
project(GameNameTBD LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/json EXCLUDE_FROM_ALL)
add_subdirectory(vendored/entt EXCLUDE_FROM_ALL)
add_subdirectory(vendored/Catch2 EXCLUDE_FROM_ALL)

# game source files

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "[/\\]main\\.cpp$")

add_library(game_core STATIC
    ${SOURCES}
)

target_link_libraries(game_core PUBLIC
    SDL3::SDL3
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)

target_include_directories(game_core PUBLIC
    src
    vendored/imgui
    vendored/imgui/backends
)

add_library(imgui STATIC
    vendored/imgui/imgui.cpp
    vendored/imgui/imgui_draw.cpp
    vendored/imgui/imgui_tables.cpp
    vendored/imgui/imgui_widgets.cpp
    vendored/imgui/backends/imgui_impl_sdl3.cpp
    vendored/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_link_libraries(imgui PRIVATE 
    SDL3::SDL3
)

target_include_directories(imgui PUBLIC
    vendored/imgui
    vendored/imgui/backends
)

add_executable(main WIN32 
    src/main.cpp
)

target_link_libraries(main PRIVATE
    game_core
    imgui
)


include(CTest)

if(BUILD_TESTING)

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        tests/*.cpp
    )

    add_executable(unit_tests
        ${TEST_SOURCES}
    )

    target_link_libraries(unit_tests PRIVATE
        game_core
        Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(unit_tests)

endif()