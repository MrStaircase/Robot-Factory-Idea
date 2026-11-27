#include <catch2/catch_test_macros.hpp>
#include <SDL3/SDL.h>
#include <cstdlib>
#include <memory>
#include <ostream>
#include <string>

#include "drawable.hpp"
#include "drawable_system.hpp"

namespace {

struct Color{
    Uint8 r, g, b, a;
};

bool operator==(const Color& lhs, const Color& rhs){
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

std::ostream& operator<<(std::ostream& os, const Color& c){
    return os << "rgba(" << int(c.r) << ", " << int(c.g) << ", " << int(c.b) << ", " << int(c.a) << ")";
}

const Color BLACK{0, 0, 0, 255};
const Color RED{255, 0, 0, 255};

struct SurfaceDeleter{
    void operator()(SDL_Surface* surface) const{
        SDL_DestroySurface(surface);
    }
};
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

Color pixel_at(SDL_Surface* surface, int x, int y){
    Color color{};
    SDL_ReadSurfacePixel(surface, x, y, &color.r, &color.g, &color.b, &color.a);
    return color;
}

// Offscreen software renderer drawing into a plain surface. No window, no GPU.
class RenderTarget{
public:
    RenderTarget(int width, int height){
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");   // lets this run headless
        REQUIRE(SDL_Init(SDL_INIT_VIDEO));
        surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
        REQUIRE(surface != nullptr);
        renderer = SDL_CreateSoftwareRenderer(surface);
        REQUIRE(renderer != nullptr);
        clear(BLACK);
    }

    ~RenderTarget(){
        SDL_DestroyRenderer(renderer);
        SDL_DestroySurface(surface);
        SDL_Quit();
    }

    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;

    SDL_Renderer& get(){ return *renderer; }

    void clear(const Color& color){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }

    // Snapshot of what has been drawn so far.
    SurfacePtr capture(){
        return SurfacePtr{SDL_RenderReadPixels(renderer, nullptr)};
    }

private:
    SDL_Surface* surface = nullptr;
    SDL_Renderer* renderer = nullptr;
};

// Drawables under test are plain functions, so no lambdas are needed.
void draw_red_square(SDL_Renderer& renderer){
    SDL_SetRenderDrawColor(&renderer, 255, 0, 0, 255);
    const SDL_FRect rect{5.0f, 5.0f, 10.0f, 10.0f};   // covers pixels 5..14
    SDL_RenderFillRect(&renderer, &rect);
}

// Golden image helper: set UPDATE_GOLDEN=1 to (re)write the reference files.
bool matches_golden(SDL_Surface* actual, const std::string& path){
    if(std::getenv("UPDATE_GOLDEN") != nullptr){
        return SDL_SaveBMP(actual, path.c_str());
    }

    SurfacePtr expected{SDL_LoadBMP(path.c_str())};
    if(!expected || expected->w != actual->w || expected->h != actual->h){
        return false;
    }
    for(int y = 0; y < actual->h; ++y){
        for(int x = 0; x < actual->w; ++x){
            if(!(pixel_at(actual, x, y) == pixel_at(expected.get(), x, y))){
                return false;
            }
        }
    }
    return true;
}

}  // namespace

TEST_CASE("Drawable draws into the renderer", "[drawable][visual]"){
    RenderTarget target{32, 32};

    Drawable drawable;
    drawable.draw = draw_red_square;
    drawable.draw(target.get());

    SurfacePtr result = target.capture();
    REQUIRE(result != nullptr);

    SECTION("pixels inside the shape are filled"){
        CHECK(pixel_at(result.get(), 5, 5) == RED);
        CHECK(pixel_at(result.get(), 10, 10) == RED);
        CHECK(pixel_at(result.get(), 14, 14) == RED);
    }

    SECTION("pixels outside the shape keep the background"){
        CHECK(pixel_at(result.get(), 0, 0) == BLACK);
        CHECK(pixel_at(result.get(), 4, 10) == BLACK);
        CHECK(pixel_at(result.get(), 15, 15) == BLACK);
    }
}

TEST_CASE("Drawable output matches the reference image", "[drawable][visual][golden]"){
    RenderTarget target{32, 32};

    Drawable drawable;
    drawable.draw = draw_red_square;
    drawable.draw(target.get());

    SurfacePtr result = target.capture();
    REQUIRE(result != nullptr);
    CHECK(matches_golden(result.get(), "golden/red_square.bmp"));
}