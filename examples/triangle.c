// This example renders a rotating triangle.
// This idea is that you can take this code and compile it to different platforms with different rendering machanisms:
// native with SDL, WebAssembly with HTML5 canvas, etc.
#include "olive.c"

#define WIDTH 800
#define HEIGHT 600
#define BACKGROUND_COLOR 0xFF181818
#define CIRCLE_RADIUS 100
#define CIRCLE_COLOR 0x99AA2020

static uint32_t pixels[WIDTH*HEIGHT];
static float triangle_angle = 0;
static float circle_x = WIDTH/2;
static float circle_y = HEIGHT/2;
static float circle_dx = 100;
static float circle_dy = 100;

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#define PI 3.14159265359

static inline void rotate_point(float *x, float *y)
{
    float dx = *x - WIDTH/2;
    float dy = *y - HEIGHT/2;
    float mag = sqrtf(dx*dx + dy*dy);
    float dir = atan2f(dy, dx) + triangle_angle;
    *x = cosf(dir)*mag + WIDTH/2;
    *y = sinf(dir)*mag + HEIGHT/2;
}

uint32_t *render(float dt)
{
    Olivec_Canvas oc = olivec_make_canvas(pixels, WIDTH, HEIGHT);

    olivec_fill(oc, BACKGROUND_COLOR);

    // Triangle
    {
        triangle_angle += 0.5f*PI*dt;

        float x1 = WIDTH/2, y1 = HEIGHT/8;
        float x2 = WIDTH/8, y2 = HEIGHT/2;
        float x3 = WIDTH*7/8, y3 = HEIGHT*7/8;
        rotate_point(&x1, &y1);
        rotate_point(&x2, &y2);
        rotate_point(&x3, &y3);
        olivec_triangle(oc, x1, y1, x2, y2, x3, y3, 0xFF2020AA);
    }

    // Circle
    {
        float x = circle_x + circle_dx*dt;
        if (x - CIRCLE_RADIUS < 0 || x + CIRCLE_RADIUS >= WIDTH) {
            circle_dx *= -1;
        } else {
            circle_x = x;
        }

        float y = circle_y + circle_dy*dt;
        if (y - CIRCLE_RADIUS < 0 || y + CIRCLE_RADIUS >= HEIGHT) {
            circle_dy *= -1;
        } else {
            circle_y = y;
        }

        olivec_circle(oc, circle_x, circle_y, CIRCLE_RADIUS, CIRCLE_COLOR);
    }

    return pixels;
}

#ifdef SDL_PLATFORM
#include <stdio.h>
#include <SDL2/SDL.h>

#define return_defer(value) do { result = (value); goto defer; } while (0)

int main(void)
{
    int result = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) return_defer(1);

        window = SDL_CreateWindow("Olivec", 0, 0, WIDTH, HEIGHT, 0);
        if (window == NULL) return_defer(1);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) return_defer(1);

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
        if (texture == NULL) return_defer(1);

        Uint32 prev = SDL_GetTicks();
        for (;;) {
            // Compute Delta Time
            Uint32 curr = SDL_GetTicks();
            float dt = (curr - prev)/1000.f;
            prev = curr;

            // Flush the events
            SDL_Event event;
            while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) return_defer(0);

            // Render the texture
            SDL_Rect window_rect = {0, 0, WIDTH, HEIGHT};
            uint32_t *pixels_src = render(dt);
            void *pixels_dst;
            int pitch;
            if (SDL_LockTexture(texture, &window_rect, &pixels_dst, &pitch) < 0) return_defer(1);
            for (size_t y = 0; y < HEIGHT; ++y) {
                memcpy(pixels_dst + y*pitch, pixels_src + y*WIDTH, WIDTH*sizeof(uint32_t));
            }
            SDL_UnlockTexture(texture);

            // Display the texture
            if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0) < 0) return_defer(1);
            if (SDL_RenderClear(renderer) < 0) return_defer(1);
            if (SDL_RenderCopy(renderer, texture, &window_rect, &window_rect) < 0) return_defer(1);
            SDL_RenderPresent(renderer);
        }
    }

defer:
    switch (result) {
    case 0:
        printf("OK\n");
        break;
    default:
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
    }
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}
#endif // SDL_PLATFORM
