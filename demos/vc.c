#define WASM_PLATFORM 0
#define SDL_PLATFORM 1
#define TERM_PLATFORM 2

#if PLATFORM == SDL_PLATFORM
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
#elif PLATFORM == TERM_PLATFORM

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

static_assert(WIDTH%SCALE_DOWN_FACTOR == 0, "WIDTH must be divisible by the SCALE_DOWN_FACTOR");
#define SCALED_DOWN_WIDTH (WIDTH/SCALE_DOWN_FACTOR)
static_assert(HEIGHT%SCALE_DOWN_FACTOR == 0, "HEIGHT must be divisible by the SCALE_DOWN_FACTOR");
#define SCALED_DOWN_HEIGHT (HEIGHT/SCALE_DOWN_FACTOR)

char char_canvas[SCALED_DOWN_WIDTH*SCALED_DOWN_HEIGHT];

char color_to_char(uint32_t pixel)
{
    size_t r = OLIVEC_RED(pixel);
    size_t g = OLIVEC_GREEN(pixel);
    size_t b = OLIVEC_BLUE(pixel);
    size_t a = OLIVEC_ALPHA(pixel);
    size_t bright = r;
    if (bright < g) bright = g;
    if (bright < b) bright = b;
    bright = bright*a/255;

    char table[] = " .:a@#";
    size_t n = sizeof(table) - 1;
    return table[bright*n/256];
}

uint32_t compress_pixels_chunk(Olivec_Canvas oc)
{
    size_t r = 0;
    size_t g = 0;
    size_t b = 0;
    size_t a = 0;

    for (size_t y = 0; y < oc.height; ++y) {
        for (size_t x = 0; x < oc.width; ++x) {
            r += OLIVEC_RED(OLIVEC_PIXEL(oc, x, y));
            g += OLIVEC_GREEN(OLIVEC_PIXEL(oc, x, y));
            b += OLIVEC_BLUE(OLIVEC_PIXEL(oc, x, y));
            a += OLIVEC_ALPHA(OLIVEC_PIXEL(oc, x, y));
        }
    }

    r /= oc.width*oc.height;
    g /= oc.width*oc.height;
    b /= oc.width*oc.height;
    a /= oc.width*oc.height;

    return OLIVEC_RGBA(r, g, b, a);
}

void compress_pixels(uint32_t *pixels)
{
    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
    for (size_t y = 0; y < SCALED_DOWN_HEIGHT; ++y) {
        for (size_t x = 0; x < SCALED_DOWN_WIDTH; ++x) {
            Olivec_Canvas soc = olivec_subcanvas(oc, x*SCALE_DOWN_FACTOR, y*SCALE_DOWN_FACTOR, SCALE_DOWN_FACTOR, 
SCALE_DOWN_FACTOR);
            char_canvas[y*SCALED_DOWN_WIDTH + x] = color_to_char(compress_pixels_chunk(soc));
        }
    }
}

int main(void)
{
    for (;;) {
        compress_pixels(render(1.f/60.f));
        for (size_t y = 0; y < SCALED_DOWN_HEIGHT; ++y) {
            for (size_t x = 0; x < SCALED_DOWN_WIDTH; ++x) {
                putc(char_canvas[y*SCALED_DOWN_WIDTH + x], stdout);
                putc(char_canvas[y*SCALED_DOWN_WIDTH + x], stdout);
            }
            putc('\n', stdout);
        }

        usleep(1000*1000/60);
        printf("\033[%dA", SCALED_DOWN_HEIGHT);
        printf("\033[%dD", SCALED_DOWN_WIDTH);
    }
    return 0;
}

#elif PLATFORM == WASM_PLATFORM
// Do nothing
#else
#error "Unknown platform"
#endif // SDL_PLATFORM
