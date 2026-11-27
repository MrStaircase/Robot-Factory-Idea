#pragma once

#include "system.hpp"
#include "drawable.hpp"

class DrawableSystem : public System{
public:
    SDL_Renderer& renderer;

    void init(Registry&) override {

    }

    void update(Registry& registry) override {
        for (entt::entity entity : registry.view<Drawable>()){
            const Drawable& drawable = registry.get<Drawable>(entity);

            drawable.draw(renderer);
        }
    }
};