#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>
#include <vector>
#include <string>
#include "lpaTPGEngine.hpp"

#define CX 8
#define CZ 8
#define CY 64

int chunk[CX][CZ][CY] = {};

void fillChunk() {
    for (int x = 0; x < CX; x++)
        for (int z = 0; z < CZ; z++)
            for (int y = 0; y < CY; y++)
                chunk[x][z][y] = rand() % 3;
}

void renderChunk(lpa::Window &win, lpa::Camera &cam) {
    lpa::Model3D mesh;

    auto isSolid = [&](int x, int y, int z) {
        if (x < 0 || x >= CX || z < 0 || z >= CZ || y < 0 || y >= CY) return false;
        return chunk[x][z][y] != 0;
    };

    for (int x = 0; x < CX; x++) {
        for (int z = 0; z < CZ; z++) {
            for (int y = 0; y < CY; y++) {
                if (!chunk[x][z][y]) continue;

                float fx = x, fy = y, fz = z;
                // top
                if (!isSolid(x, y+1, z)) {
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy+1,fz), lpa::Vec3(fx+1,fy+1,fz+1), lpa::Vec3(fx+1,fy+1,fz)));
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy+1,fz), lpa::Vec3(fx,fy+1,fz+1), lpa::Vec3(fx+1,fy+1,fz+1)));
                }
                // bottom
                if (!isSolid(x, y-1, z)) {
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy,fz+1), lpa::Vec3(fx+1,fy,fz), lpa::Vec3(fx+1,fy,fz+1)));
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy,fz+1), lpa::Vec3(fx,fy,fz), lpa::Vec3(fx+1,fy,fz)));
                }
                // front
                if (!isSolid(x, y, z+1)) {
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy,fz+1), lpa::Vec3(fx+1,fy+1,fz+1), lpa::Vec3(fx,fy+1,fz+1)));
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy,fz+1), lpa::Vec3(fx+1,fy,fz+1), lpa::Vec3(fx+1,fy+1,fz+1)));
                }
                // back
                if (!isSolid(x, y, z-1)) {
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx+1,fy,fz), lpa::Vec3(fx,fy+1,fz), lpa::Vec3(fx+1,fy+1,fz)));
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx+1,fy,fz), lpa::Vec3(fx,fy,fz), lpa::Vec3(fx,fy+1,fz)));
                }
                // right
                if (!isSolid(x+1, y, z)) {
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx+1,fy,fz+1), lpa::Vec3(fx+1,fy+1,fz), lpa::Vec3(fx+1,fy+1,fz+1)));
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx+1,fy,fz+1), lpa::Vec3(fx+1,fy,fz), lpa::Vec3(fx+1,fy+1,fz)));
                }
                // left
                if (!isSolid(x-1, y, z)) {
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy,fz), lpa::Vec3(fx,fy+1,fz+1), lpa::Vec3(fx,fy+1,fz)));
                    mesh.addTri(lpa::triangle3d(lpa::Vec3(fx,fy,fz), lpa::Vec3(fx,fy,fz+1), lpa::Vec3(fx,fy+1,fz+1)));
                }
            }
        }
    }

    mesh.render(win, lpa::color::GREEN, cam);
}

int main() {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    lpa::Window win("Chunk Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600);
    IMG_Init(IMG_INIT_PNG);

    lpa::Camera cam(0, 5.0f, -5.0f, 400.0f, 800, 600);
    lpa::Font font("./ASSETS-(for-example)/OpenSans-Regular.ttf", 24);
    if (!font.font) printf("Font failed to load: %s\n", TTF_GetError());

    fillChunk();

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
        win.drawSkybox(lpa::color::CYAN, lpa::color::bright::GREEN, cam);
        renderChunk(win, cam);
        std::string fpsStr = "FPS: " + std::to_string((int)(1.0f / dt));
        win.drawText(fpsStr.c_str(), 10, 10, font, lpa::color::BLACK);
        win.present();
    }

    IMG_Quit();
    return 0;
}