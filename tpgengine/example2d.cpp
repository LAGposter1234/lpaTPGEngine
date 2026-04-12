#include "lpaTPGEngine.hpp"
#include <SDL2/SDL.h>
#include <string>

int main() {
    lpa::Window win("2d Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600);
    lpa::Font font("ASSETS-(for-example)/OpenSans-Regular.ttf", 24);

    lpa::Vec2 pos(0,0);
    lpa::Vec2 vel(0,0);

    lpa::Vec2 campos(0,0);

    bool running = true;
    SDL_Event event;
    bool up;
    bool down;
    bool left;
    bool right;
    float lastTime;
    while (running) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTime) / 1000.0f;
        lastTime = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: up = true; break;
                    case SDLK_DOWN: down = true; break;
                    case SDLK_LEFT: left = true; break;
                    case SDLK_RIGHT: right = true; break;
                    case SDLK_ESCAPE: running = false; break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: up = false; break;
                    case SDLK_DOWN: down = false; break;
                    case SDLK_LEFT: left = false; break;
                    case SDLK_RIGHT: right = false; break;
                }
            }
        }

        if (up) vel.y -= 1;
        if (down) vel.y += 1;
        if (left) vel.x -= 1;
        if (right) vel.x += 1;

        vel.x *= 0.99;
        vel.y *= 0.99;

        pos.x += (vel.x / 10) * dt;
        pos.y += (vel.y / 10) * dt;

        campos.x += (pos.x - campos.x) / 20;
        campos.y += (pos.y - campos.y) / 20;

        win.clear(lpa::color::WHITE);
        std::string x = "X: " + std::to_string(campos.x);
        std::string y = "Y: " + std::to_string(campos.y);
        std::string fps = "FPS: " + std::to_string(1 / dt);
        win.drawText(x.c_str(), 10, 10, font, lpa::color::BLACK);
        win.drawText(y.c_str(), 10, 35, font, lpa::color::BLACK);
        win.drawText(fps.c_str(), 10, 60, font, lpa::color::BLACK);

        win.fillRect((pos.x - campos.x) + 383, (pos.y - campos.y) + 283, 33.33f, 33.33f, lpa::color::BLUE);
        win.present();
    }

    return 0;
}