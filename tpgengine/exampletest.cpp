#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>
#include <vector>
#include <string>
#include "lpaTPGEngine.hpp"

int main() {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    lpa::Window win("TPG Engine Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600);
    IMG_Init(IMG_INIT_PNG);

    lpa::Camera cam(0, 0, -5.0f, 400.0f, 800, 600);

    lpa::Model3D model = lpa::Model3D::loadFromFile("suzanne.bin");
    lpa::Model3D pyramid = lpa::Model3D::loadFromFile("pyramid.bin");
    lpa::Model3D gcube = lpa::Model3D::cube(1);
    lpa::Model3D wcube = lpa::Model3D::cube(1);
    lpa::Model3D bcube = lpa::Model3D::cube(1);

    gcube.translate(3.0f, 0.0f, 0.0f);
    wcube.translate(3.0f, 1.0f, 0.0f);
    bcube.translate(3.0f, -1.0f, 0.0f);
    model.rotateX(67.5f);
    pyramid.translate(-3.0f, 1.0f, 0.0f);
    pyramid.rotateX(67.5f);

    lpa::Font font("./ASSETS-(for-example)/OpenSans-Regular.ttf", 24);
    if (!font.font) printf("Font failed to load: %s\n", TTF_GetError());

    float speed = 3.0f;
    float rotSpeed = 1.5f;
    bool running = true;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTime) / 1000.0f;
        lastTime = now;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        if (win.keyPressed(SDLK_w)) cam.moveForward(speed * dt);
        if (win.keyPressed(SDLK_s)) cam.moveBack(speed * dt);
        if (win.keyPressed(SDLK_a)) cam.moveLeft(speed * dt);
        if (win.keyPressed(SDLK_d)) cam.moveRight(speed * dt);
        if (win.keyPressed(SDLK_q)) cam.moveUp(speed * dt);
        if (win.keyPressed(SDLK_e)) cam.moveDown(speed * dt);
        if (win.keyPressed(SDLK_LEFT))  cam.yaw   += rotSpeed * dt;
        if (win.keyPressed(SDLK_RIGHT)) cam.yaw   -= rotSpeed * dt;
        if (win.keyPressed(SDLK_UP))    cam.pitch -= rotSpeed * dt;
        if (win.keyPressed(SDLK_DOWN))  cam.pitch += rotSpeed * dt;

        win.clear(lpa::color::BLACK);
        win.drawSkybox(lpa::color::bright::BLUE, lpa::color::GREEN, cam);
        model.render(win, lpa::color::PURPLE, cam);
        win.drawText("Hello, World! Enjoy the demo!", 10, 10, font, lpa::color::BLACK);
        win.drawText("WASD - Move around in X and Z! Arrow keys - Turn camera!", 10, 35, font, lpa::color::BLACK);
        std::string fpsStr = "FPS: " + std::to_string((int)(1.0f / dt));
        win.drawText(fpsStr.c_str(), 10, 60, font, lpa::color::BLACK);
        win.present();
    }

    IMG_Quit();
    return 0;
}