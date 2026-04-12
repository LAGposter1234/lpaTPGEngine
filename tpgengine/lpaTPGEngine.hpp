#ifndef TPGENGINE_LPATPGENGINE_H
#define TPGENGINE_LPATPGENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <string>
#include <cmath>
#include <algorithm>
#include <SDL2/SDL_ttf.h>

namespace lpa {
    class Font {
    public:
        TTF_Font *font;

        Font() : font(nullptr) {}
        Font(const char *path, int size) {
            font = TTF_OpenFont(path, size);
        }

        ~Font() {
            TTF_CloseFont(font);
        }
    };

    class Vec2 {
    public:
        float x, y;
        Vec2() : x(0), y(0) {}
        Vec2(float x, float y) : x(x), y(y) {}
        inline void setVec(float x, float y) {
            this->x = x;
            this->y = y;
        }
    };

    class Camera {
    public:
        float x, y, z;
        float yaw, pitch;
        float fov;
        int screenW, screenH;

        Camera() : x(0), y(0), z(-3.0f), yaw(0), pitch(0), fov(400.0f), screenW(800), screenH(600) {}
        Camera(float x, float y, float z, float fov, int screenW, int screenH)
            : x(x), y(y), z(z), yaw(0), pitch(0), fov(fov), screenW(screenW), screenH(screenH) {}

        void moveForward(float speed) { x -= sin(yaw) * speed; z += cos(yaw) * speed; }
        void moveBack(float speed)    { x += sin(yaw) * speed; z -= cos(yaw) * speed; }
        void moveLeft(float speed)    { x -= cos(yaw) * speed; z -= sin(yaw) * speed; }
        void moveRight(float speed)   { x += cos(yaw) * speed; z += sin(yaw) * speed; }
        void moveUp(float speed)      { y += speed; }
        void moveDown(float speed)    { y -= speed; }
    };

    class Vec3 {
    public:
        float x, y, z;
        Vec3() : x(0), y(0), z(0) {}
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
        inline void setVec3(float x, float y, float z) {
            this->x = x; this->y = y; this->z = z;
        }
        inline Vec2 toVec2(Camera &cam, bool &behind) {
            float rx = x - cam.x;
            float ry = y - cam.y;
            float rz = z - cam.z;

            float x1 =  rx * cos(cam.yaw) + rz * sin(cam.yaw);
            float z1 = -rx * sin(cam.yaw) + rz * cos(cam.yaw);
            float y1 =  ry * cos(cam.pitch) - z1 * sin(cam.pitch);
            float z2 =  ry * sin(cam.pitch) + z1 * cos(cam.pitch);

            if (z2 <= 0.01f) { behind = true; return Vec2(0, 0); }
            behind = false;
            return Vec2(
                (x1 / z2) * cam.fov + cam.screenW / 2,
                (y1 / z2) * cam.fov + cam.screenH / 2
            );
        }
    };

    class Color {
    public:
        uint8_t r, g, b, a;
        Color() : r(0), g(0), b(0), a(255) {}
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
        SDL_Color toSDL() { return {r, g, b, a}; }
    };

    namespace color {
        inline Color RED     = Color(255, 0,   0);
        inline Color GREEN   = Color(0,   255, 0);
        inline Color BLUE    = Color(0,   0,   255);
        inline Color YELLOW  = Color(255, 255, 0);
        inline Color ORANGE  = Color(255, 165, 0);
        inline Color PURPLE  = Color(128, 0,   128);
        inline Color CYAN    = Color(0,   255, 255);
        inline Color MAGENTA = Color(255, 0,   255);
        inline Color BLACK   = Color(0,   0,   0);
        inline Color WHITE   = Color(255, 255, 255);

        namespace bright {
            inline Color RED     = Color(255, 127, 127);
            inline Color GREEN   = Color(127, 255, 127);
            inline Color BLUE    = Color(127, 127, 255);
            inline Color YELLOW  = Color(255, 255, 127);
            inline Color ORANGE  = Color(255, 185, 127);
            inline Color PURPLE  = Color(178, 127, 255);
            inline Color CYAN    = Color(137, 207, 240);
            inline Color MAGENTA = Color(255, 127, 255);
            inline Color GREY    = Color(128, 128, 128);
        }
    }

    class triangle2d {
    public:
        Vec2 p1, p2, p3;
        triangle2d() {}
        triangle2d(Vec2 p1, Vec2 p2, Vec2 p3) : p1(p1), p2(p2), p3(p3) {}
    };

    class triangle3d {
    public:
        Vec3 p1, p2, p3;
        triangle3d() {}
        triangle3d(Vec3 p1, Vec3 p2, Vec3 p3) : p1(p1), p2(p2), p3(p3) {}
        Vec3 normal() {
            Vec3 u(p2.x-p1.x, p2.y-p1.y, p2.z-p1.z);
            Vec3 v(p3.x-p1.x, p3.y-p1.y, p3.z-p1.z);
            Vec3 n(
                u.y*v.z - u.z*v.y,
                u.z*v.x - u.x*v.z,
                u.x*v.y - u.y*v.x
            );
            float len = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
            if (len == 0) return Vec3(0, 0, 0);
            return Vec3(n.x/len, n.y/len, n.z/len);
        }
    };

    triangle3d atot(const char* ascii) {
        lpa::triangle3d null;
        if (!ascii || ascii[0] != '(') return null;

        float vals[9];
        int v = 0, i = 1, b = 0;
        char buf[64];

        while (ascii[i] != '\0' && v < 9) {
            char c = ascii[i];
            if (c == ',' || c == ')' || c == '(') {
                if (b > 0) { buf[b] = '\0'; vals[v++] = atof(buf); b = 0; }
            } else if (c != ' ') {
                buf[b++] = c;
            }
            i++;
        }

        if (v < 9) return null;
        return lpa::triangle3d(
            lpa::Vec3(vals[0], vals[1], vals[2]),
            lpa::Vec3(vals[3], vals[4], vals[5]),
            lpa::Vec3(vals[6], vals[7], vals[8])
        );
    }

    triangle3d stot(std::string s) { return atot(s.c_str()); }

    class Costume {
    public:
        SDL_Texture *texture;
        int w, h;

        Costume() : texture(nullptr), w(0), h(0) {}
        Costume(SDL_Renderer *renderer, const char *path) : texture(nullptr), w(0), h(0) {
            SDL_Surface *surf = IMG_Load(path);
            texture = SDL_CreateTextureFromSurface(renderer, surf);
            w = surf->w; h = surf->h;
            SDL_FreeSurface(surf);
        }
        ~Costume() { SDL_DestroyTexture(texture); }
    };

    class TileSet {
        public:
        std::vector<Costume> costumes;
        TileSet() {
            costumes = {};
        }
        void addCostume(Costume cos) { costumes.push_back(cos); }
        void removeCostume(Costume cos) { costumes.pop_back(); }
    };

    class TileGrid {
        public:
        TileSet *tileset;
        std::vector<std::vector<int>> grid;
        TileGrid() {
            tileset = new TileSet();
            grid = {};
        }
        int get(int x, int y) {
            return grid[x][y];
        }
        void set(int x, int y, int data) {
            grid[x][y] = data;
        }
        void pushLayer(std::vector<int> layer) {
            grid.push_back(layer);
        }
        void pushSingle(int y, int data) {
            grid[y].push_back(data);
        }
    };

    class Window {
    public:
        SDL_Window   *window;
        SDL_Renderer *renderer;
        int screenW, screenH;
        SDL_Texture  *pixelBuffer;
        uint32_t     *pixels;
        int           pitch;

        Window() : window(nullptr), renderer(nullptr), pixelBuffer(nullptr), pixels(nullptr), pitch(0) {}

        Window(const char *title, int x, int y, int w, int h)
            : window(nullptr), renderer(nullptr), pixelBuffer(nullptr), pixels(nullptr), pitch(0) {
            screenW = w; screenH = h;
            SDL_Init(SDL_INIT_EVERYTHING);
            TTF_Init();
            window   = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
            pixelBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_STREAMING, w, h);
            SDL_RenderClear(renderer);
        }

        ~Window() {
            if (pixelBuffer) SDL_DestroyTexture(pixelBuffer);
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        inline uint32_t packColor(Color c) {
            return ((uint32_t)c.r << 24) | ((uint32_t)c.g << 16) | ((uint32_t)c.b << 8) | c.a;
        }

        void setColor(Color c) {
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        }

        void clear(Color c = color::BLACK) {
            SDL_LockTexture(pixelBuffer, nullptr, (void**)&pixels, &pitch);
            uint32_t col = packColor(c);
            int count = (pitch / 4) * screenH;
            for (int i = 0; i < count; i++) pixels[i] = col;
        }

        void present() {
            SDL_UnlockTexture(pixelBuffer);
            SDL_RenderCopy(renderer, pixelBuffer, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        void drawPoint(int x, int y, Color c) {
            if (x < 0 || x >= screenW || y < 0 || y >= screenH) return;
            pixels[y * (pitch / 4) + x] = packColor(c);
        }

        void drawLine(int x1, int y1, int x2, int y2, Color c) {
            uint32_t col = packColor(c);
            int dx = abs(x2-x1), dy = abs(y2-y1);
            int sx = x1 < x2 ? 1 : -1;
            int sy = y1 < y2 ? 1 : -1;
            int err = dx - dy;
            while (true) {
                if (x1 >= 0 && x1 < screenW && y1 >= 0 && y1 < screenH)
                    pixels[y1 * (pitch / 4) + x1] = col;
                if (x1 == x2 && y1 == y2) break;
                int e2 = 2 * err;
                if (e2 > -dy) { err -= dy; x1 += sx; }
                if (e2 <  dx) { err += dx; y1 += sy; }
            }
        }

        void drawRect(int x, int y, int w, int h, Color c) {
            drawLine(x,   y,   x+w, y,   c);
            drawLine(x+w, y,   x+w, y+h, c);
            drawLine(x+w, y+h, x,   y+h, c);
            drawLine(x,   y+h, x,   y,   c);
        }

        void fillRect(int x, int y, int w, int h, Color c) {
            uint32_t col = packColor(c);
            int x1 = x   < 0 ? 0 : x   > screenW ? screenW : x;
            int y1 = y   < 0 ? 0 : y   > screenH ? screenH : y;
            int x2 = x+w < 0 ? 0 : x+w > screenW ? screenW : x+w;
            int y2 = y+h < 0 ? 0 : y+h > screenH ? screenH : y+h;
            for (int py = y1; py < y2; py++) {
                uint32_t *row = pixels + py * (pitch / 4) + x1;
                for (int px = x1; px < x2; px++) *row++ = col;
            }
        }

        void drawTriangle(triangle2d t, Color c) {
            drawLine(t.p1.x, t.p1.y, t.p2.x, t.p2.y, c);
            drawLine(t.p2.x, t.p2.y, t.p3.x, t.p3.y, c);
            drawLine(t.p3.x, t.p3.y, t.p1.x, t.p1.y, c);
        }

        void drawTriangle3d(triangle3d t, Color c, Camera &cam) {
            bool b1, b2, b3;
            Vec2 p1 = t.p1.toVec2(cam, b1);
            Vec2 p2 = t.p2.toVec2(cam, b2);
            Vec2 p3 = t.p3.toVec2(cam, b3);
            if (b1 || b2 || b3) return;
            drawTriangle(triangle2d(p1, p2, p3), c);
        }

        void fillTriangle(triangle2d t, Color c) {
            Vec2 p1 = t.p1, p2 = t.p2, p3 = t.p3;
            if (p2.y < p1.y) { Vec2 tmp = p1; p1 = p2; p2 = tmp; }
            if (p3.y < p1.y) { Vec2 tmp = p1; p1 = p3; p3 = tmp; }
            if (p3.y < p2.y) { Vec2 tmp = p2; p2 = p3; p3 = tmp; }

            int y1 = (int)p1.y, y2 = (int)p2.y, y3 = (int)p3.y;
            if (y3 < 0 || y1 > screenH || y1 == y3) return;

            uint32_t col = packColor(c);

            float invSlope1  = (p3.x - p1.x) / (y3 - y1);
            float invSlope2a = (y2 != y1) ? (p2.x - p1.x) / (y2 - y1) : 0;
            float invSlope2b = (y3 != y2) ? (p3.x - p2.x) / (y3 - y2) : 0;

            float xA = p1.x;
            float xB = p1.x;

            for (int y = y1; y <= y3; y++) {
                if (y == y2) xB = p2.x;

                if (y >= 0 && y < screenH) {
                    int xa = (int)fmin(xA, xB);
                    int xb = (int)fmax(xA, xB);
                    xa = xa < 0 ? 0 : xa > screenW ? screenW : xa;
                    xb = xb < 0 ? 0 : xb > screenW ? screenW : xb;
                    uint32_t *row = pixels + y * (pitch / 4) + xa;
                    for (int x = xa; x < xb; x++) *row++ = col;
                }

                xA += invSlope1;
                if (y < y2) xB += invSlope2a;
                else        xB += invSlope2b;
            }
        }

        // this sed to be alot more complex
        void fillTriangle3d(triangle3d t, Color c, Camera &cam) {
            bool b1, b2, b3;
            Vec2 p1 = t.p1.toVec2(cam, b1);
            Vec2 p2 = t.p2.toVec2(cam, b2);
            Vec2 p3 = t.p3.toVec2(cam, b3);
            if (b1 || b2 || b3) return;
            fillTriangle(triangle2d(p1, p2, p3), c);
        }
        // wip
        void drawSkybox(Color top, Color bottom, Camera &cam) {
            float pitchOffset = (cam.pitch / 3.14159265f) * 2.0f;
            for (int y = 0; y < screenH; y++) {
                float t = ((float)y / screenH - 0.5f + pitchOffset) / 2.0f + 0.5f;
                t = fmod(t, 1.0f);
                if (t < 0) t += 1.0f;
                Color c(
                    (uint8_t)(top.r + t * (bottom.r - top.r)),
                    (uint8_t)(top.g + t * (bottom.g - top.g)),
                    (uint8_t)(top.b + t * (bottom.b - top.b))
                );
                uint32_t col = packColor(c);
                uint32_t *row = pixels + y * (pitch / 4);
                for (int x = 0; x < screenW; x++) *row++ = col;
            }
        }

        void drawText(const char *text, int x, int y, Font &font, Color c) {
            SDL_Color col = c.toSDL();
            SDL_Surface *surf = TTF_RenderText_Solid(font.font, text, col);
            if (!surf) return;
            SDL_Surface *conv = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA8888, 0);
            SDL_FreeSurface(surf);
            if (!conv) return;

            SDL_LockSurface(conv);
            uint32_t *src = (uint32_t *)conv->pixels;
            int srcPitch = conv->pitch / 4;

            for (int py = 0; py < conv->h; py++) {
                int dy = y + py;
                if (dy < 0 || dy >= screenH) continue;
                for (int px = 0; px < conv->w; px++) {
                    int dx = x + px;
                    if (dx < 0 || dx >= screenW) continue;
                    uint32_t pixel = src[py * srcPitch + px];
                    if ((pixel & 0xFF) != 0)
                        pixels[dy * (pitch / 4) + dx] = pixel;
                }
            }

            SDL_UnlockSurface(conv);
            SDL_FreeSurface(conv);
        }

        void drawCostume(Costume &cos, int x, int y) {
            SDL_UnlockTexture(pixelBuffer);
            SDL_Rect dst = {x, y, cos.w, cos.h};
            SDL_RenderCopy(renderer, cos.texture, nullptr, &dst);
            SDL_LockTexture(pixelBuffer, nullptr, (void**)&pixels, &pitch);
        }

        void drawCostume(Costume &cos, int x, int y, int w, int h) {
            SDL_UnlockTexture(pixelBuffer);
            SDL_Rect dst = {x, y, w, h};
            SDL_RenderCopy(renderer, cos.texture, nullptr, &dst);
            SDL_LockTexture(pixelBuffer, nullptr, (void**)&pixels, &pitch);
        }

        bool keyPressed(SDL_Keycode key) {
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            return state[SDL_GetScancodeFromKey(key)];
        }
    };

    class Polygon2D {
    public:
        std::vector<Vec2> points;

        Polygon2D() {}

        void addPoint(Vec2 p) { points.push_back(p); }

        void draw(Window &win, Color c) {
            for (int i = 0; i < (int)points.size(); i++) {
                Vec2 &a = points[i];
                Vec2 &b = points[(i + 1) % points.size()];
                win.drawLine(a.x, a.y, b.x, b.y, c);
            }
        }
    };

    class Model3D {
    public:
        std::vector<triangle3d> tris;
        Costume *costume;

        Model3D() : costume(nullptr) {}

        void translate(float tx, float ty, float tz) {
            for (triangle3d &t : tris) {
                t.p1.x += tx; t.p1.y += ty; t.p1.z += tz;
                t.p2.x += tx; t.p2.y += ty; t.p2.z += tz;
                t.p3.x += tx; t.p3.y += ty; t.p3.z += tz;
            }
        }

        void rotateX(float angle) {
            float cx = 0, cy = 0, cz = 0;
            for (triangle3d &t : tris) {
                cx += t.p1.x + t.p2.x + t.p3.x;
                cy += t.p1.y + t.p2.y + t.p3.y;
                cz += t.p1.z + t.p2.z + t.p3.z;
            }
            int n = tris.size() * 3;
            cx /= n; cy /= n; cz /= n;
            for (triangle3d &t : tris) {
                for (Vec3 *p : {&t.p1, &t.p2, &t.p3}) {
                    float y = (p->y-cy)*cos(angle) - (p->z-cz)*sin(angle);
                    float z = (p->y-cy)*sin(angle) + (p->z-cz)*cos(angle);
                    p->y = cy+y; p->z = cz+z;
                }
            }
        }

        void rotateY(float angle) {
            float cx = 0, cy = 0, cz = 0;
            for (triangle3d &t : tris) {
                cx += t.p1.x + t.p2.x + t.p3.x;
                cy += t.p1.y + t.p2.y + t.p3.y;
                cz += t.p1.z + t.p2.z + t.p3.z;
            }
            int n = tris.size() * 3;
            cx /= n; cy /= n; cz /= n;
            for (triangle3d &t : tris) {
                for (Vec3 *p : {&t.p1, &t.p2, &t.p3}) {
                    float x =  (p->x-cx)*cos(angle) + (p->z-cz)*sin(angle);
                    float z = -(p->x-cx)*sin(angle) + (p->z-cz)*cos(angle);
                    p->x = cx+x; p->z = cz+z;
                }
            }
        }

        void rotateZ(float angle) {
            float cx = 0, cy = 0, cz = 0;
            for (triangle3d &t : tris) {
                cx += t.p1.x + t.p2.x + t.p3.x;
                cy += t.p1.y + t.p2.y + t.p3.y;
                cz += t.p1.z + t.p2.z + t.p3.z;
            }
            int n = tris.size() * 3;
            cx /= n; cy /= n; cz /= n;
            for (triangle3d &t : tris) {
                for (Vec3 *p : {&t.p1, &t.p2, &t.p3}) {
                    float x = (p->x-cx)*cos(angle) - (p->y-cy)*sin(angle);
                    float y = (p->x-cx)*sin(angle) + (p->y-cy)*cos(angle);
                    p->x = cx+x; p->y = cy+y;
                }
            }
        }

        void addTri(triangle3d t) { tris.push_back(t); }

        void draw(Window &win, Color c, Camera cam) {
            for (triangle3d &t : tris) win.drawTriangle3d(t, c, cam);
        }

        void fill(Window &win, Color c, Camera &cam) {
            for (triangle3d &t : tris) win.fillTriangle3d(t, c, cam);
        }

        void render(Window &win, Color c, Camera &cam, Vec3 *light = nullptr) {
            std::vector<triangle3d> sorted = tris;
            std::sort(sorted.begin(), sorted.end(), [&](const triangle3d &a, const triangle3d &b) {
                auto dist = [&](const triangle3d &t) {
                    float cx = (t.p1.x+t.p2.x+t.p3.x)/3.0f - cam.x;
                    float cy = (t.p1.y+t.p2.y+t.p3.y)/3.0f - cam.y;
                    float cz = (t.p1.z+t.p2.z+t.p3.z)/3.0f - cam.z;
                    return sqrt(cx*cx + cy*cy + cz*cz);
                };
                return dist(a) > dist(b);
            });

            for (triangle3d &t : sorted) {
                Vec3 n = t.normal();
                Vec3 center((t.p1.x+t.p2.x+t.p3.x)/3.0f,
                            (t.p1.y+t.p2.y+t.p3.y)/3.0f,
                            (t.p1.z+t.p2.z+t.p3.z)/3.0f);
                Vec3 camDir(cam.x-center.x, cam.y-center.y, cam.z-center.z);
                float facing = n.x*camDir.x + n.y*camDir.y + n.z*camDir.z;
                if (facing <= 0) continue;

                Vec3 lp = light ? *light : Vec3(cam.x, cam.y, cam.z);
                Vec3 lightDir(lp.x-center.x, lp.y-center.y, lp.z-center.z);
                float len = sqrt(lightDir.x*lightDir.x + lightDir.y*lightDir.y + lightDir.z*lightDir.z);
                if (len == 0) continue;
                lightDir.x /= len; lightDir.y /= len; lightDir.z /= len;

                float dot = n.x*lightDir.x + n.y*lightDir.y + n.z*lightDir.z;
                if (dot < 0) dot = 0;
                float bright = 0.1f + dot * 0.9f;
                Color lit((uint8_t)(c.r*bright), (uint8_t)(c.g*bright), (uint8_t)(c.b*bright), c.a);
                win.fillTriangle3d(t, lit, cam);
            }
        }

        static void renderAll(Window &win, Camera &cam, std::vector<std::pair<Model3D*, Color>> models, Vec3 *light = nullptr) {
            std::vector<std::pair<triangle3d, Color>> allTris;

            for (auto &pair : models)
                for (triangle3d &t : pair.first->tris)
                    allTris.push_back({t, pair.second});

            std::sort(allTris.begin(), allTris.end(), [&](const auto &a, const auto &b) {
                auto dist = [&](const triangle3d &t) {
                    float cx = (t.p1.x+t.p2.x+t.p3.x)/3.0f - cam.x;
                    float cy = (t.p1.y+t.p2.y+t.p3.y)/3.0f - cam.y;
                    float cz = (t.p1.z+t.p2.z+t.p3.z)/3.0f - cam.z;
                    return sqrt(cx*cx + cy*cy + cz*cz);
                };
                return dist(a.first) > dist(b.first);
            });

            for (auto &pair : allTris) {
                triangle3d &t = pair.first;
                Color &c = pair.second;

                Vec3 n = t.normal();
                Vec3 center((t.p1.x+t.p2.x+t.p3.x)/3.0f,
                            (t.p1.y+t.p2.y+t.p3.y)/3.0f,
                            (t.p1.z+t.p2.z+t.p3.z)/3.0f);
                Vec3 camDir(cam.x-center.x, cam.y-center.y, cam.z-center.z);
                float facing = n.x*camDir.x + n.y*camDir.y + n.z*camDir.z;
                if (facing <= 0) continue;

                Vec3 lp = light ? *light : Vec3(cam.x, cam.y, cam.z);
                Vec3 lightDir(lp.x-center.x, lp.y-center.y, lp.z-center.z);
                float len = sqrt(lightDir.x*lightDir.x + lightDir.y*lightDir.y + lightDir.z*lightDir.z);
                if (len == 0) continue;
                lightDir.x /= len; lightDir.y /= len; lightDir.z /= len;

                float dot = n.x*lightDir.x + n.y*lightDir.y + n.z*lightDir.z;
                if (dot < 0) dot = 0;
                float bright = 0.1f + dot * 0.9f;
                Color lit((uint8_t)(c.r*bright), (uint8_t)(c.g*bright), (uint8_t)(c.b*bright), c.a);
                win.fillTriangle3d(t, lit, cam);
            }
        }

        static Model3D cube(float size) {
            Model3D m;
            float h = size / 2;
            m.addTri(triangle3d(Vec3(-h,-h, h), Vec3( h,-h, h), Vec3( h, h, h)));
            m.addTri(triangle3d(Vec3(-h,-h, h), Vec3( h, h, h), Vec3(-h, h, h)));
            m.addTri(triangle3d(Vec3(-h,-h,-h), Vec3( h, h,-h), Vec3( h,-h,-h)));
            m.addTri(triangle3d(Vec3(-h,-h,-h), Vec3(-h, h,-h), Vec3( h, h,-h)));
            m.addTri(triangle3d(Vec3(-h, h,-h), Vec3(-h, h, h), Vec3( h, h, h)));
            m.addTri(triangle3d(Vec3(-h, h,-h), Vec3( h, h, h), Vec3( h, h,-h)));
            m.addTri(triangle3d(Vec3(-h,-h,-h), Vec3( h,-h, h), Vec3(-h,-h, h)));
            m.addTri(triangle3d(Vec3(-h,-h,-h), Vec3( h,-h,-h), Vec3( h,-h, h)));
            m.addTri(triangle3d(Vec3( h,-h,-h), Vec3( h, h, h), Vec3( h,-h, h)));
            m.addTri(triangle3d(Vec3( h,-h,-h), Vec3( h, h,-h), Vec3( h, h, h)));
            m.addTri(triangle3d(Vec3(-h,-h,-h), Vec3(-h,-h, h), Vec3(-h, h, h)));
            m.addTri(triangle3d(Vec3(-h,-h,-h), Vec3(-h, h, h), Vec3(-h, h,-h)));
            return m;
        }

        static Model3D loadFromFile(const char *path) {
            Model3D m;
            FILE *f = fopen(path, "rb");
            if (!f) return m;
            uint32_t count;
            fread(&count, sizeof(uint32_t), 1, f);
            for (uint32_t i = 0; i < count; i++) {
                triangle3d t;
                fread(&t, sizeof(triangle3d), 1, f);
                m.addTri(t);
            }
            fclose(f);
            return m;
        }

        void saveToFile(const char *path) {
            FILE *f = fopen(path, "wb");
            if (!f) return;
            uint32_t count = tris.size();
            fwrite(&count, sizeof(uint32_t), 1, f);
            for (triangle3d &t : tris) fwrite(&t, sizeof(triangle3d), 1, f);
            fclose(f);
        }
    };
}

#endif //TPGENGINE_LPATPGENGINE_H