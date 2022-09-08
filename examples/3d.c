// This example renders a rotating triangle.
// This idea is that you can take this code and compile it to different platforms with different rendering machanisms:
// native with SDL, WebAssembly with HTML5 canvas, etc.
#define OLIVEC_IMPLEMENTATION
#include "olive.c"

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#define PI 3.14159265359

// #define FACTOR 100
// #define WIDTH (16*FACTOR)
// #define HEIGHT (9*FACTOR)
#define WIDTH 800
#define HEIGHT 600
#define BACKGROUND_COLOR 0xFF181818
#define GRID_COUNT 10
#define GRID_PAD 0.5/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define CIRCLE_RADIUS 5
#define Z_START 0.25
#define ABOBA_PADDING 50

uint32_t circle_colors[] = {
    0xFF2020FF,
    0xFF20FF20,
    0xFFFF2020,
    0xFF20FFFF,
    0xFFFF20FF,
    0xFFFFFF20,
};
#define circle_colors_count (sizeof(circle_colors)/sizeof(circle_colors[0]))

static uint32_t pixels[WIDTH*HEIGHT];
static float angle = 0;

uint32_t *render(float dt)
{
    angle += 0.25*PI*dt;

    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT);

    olivec_fill(oc, BACKGROUND_COLOR);
    for (int ix = 0; ix < GRID_COUNT; ++ix) {
        for (int iy = 0; iy < GRID_COUNT; ++iy) {
            for (int iz = 0; iz < GRID_COUNT; ++iz) {
                float x = ix*GRID_PAD - GRID_SIZE/2;
                float y = iy*GRID_PAD - GRID_SIZE/2;
                float z = Z_START + iz*GRID_PAD;

                float cx = 0.0;
                float cz = Z_START + GRID_SIZE/2;

                float dx = x - cx;
                float dz = z - cz;

                float a = atan2f(dz, dx);
                float m = sqrtf(dx*dx + dz*dz);

                dx = cosf(a + angle)*m;
                dz = sinf(a + angle)*m;

                x = dx + cx;
                z = dz + cz;

                x /= z;
                y /= z;

                uint32_t r = ix*255/GRID_COUNT;
                uint32_t g = iy*255/GRID_COUNT;
                uint32_t b = iz*255/GRID_COUNT;
                uint32_t color = 0xFF000000 | (r<<(0*8)) | (g<<(1*8)) | (b<<(2*8));
                olivec_circle(oc, (x + 1)/2*WIDTH, (y + 1)/2*HEIGHT, CIRCLE_RADIUS, color);
            }
        }
    }

    size_t size = 8;
    olivec_text(oc, "aboba", ABOBA_PADDING, HEIGHT - ABOBA_PADDING - default_font.height*size, default_font, size, 0xFFFFFFFF);

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
