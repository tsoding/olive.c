// C implementation of the Virtual Console (VC) for demos.
//
// # Usage
// ```c
// // demo.c
// // vc.c expectes render() to be defined and also supplies it's own entry point
// // if needed (some platforms like WASM_PLATFORM do not have the main()
// // entry point)
// #include "vc.c"
//
// #define WIDTH 800
// #define HEIGHT 600
// static uint32_t pixels[WIDTH*HEIGHT];
//
// static Olivec_Canvas vc_render(float dt)
// {
//     Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
//     // ...
//     // ... render into oc ...
//     // ...
//     return oc;
// }
// ```
//
// # Build
// ```console
// $ clang -o demo.sdl -DVC_PLATFORM=VC_SDL_PLATFORM demo.c -lSDL2
// $ clang -o demo.term -DVC_PLATFORM=VC_TERM_PLATFORM demo.c
// $ clang -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--allow-undefined -o demo.wasm -DVC_PLATFORM=VC_WASM_PLATFORM demo.c
// ```

#define OLIVEC_IMPLEMENTATION
#include <olive.c>

Olivec_Canvas vc_render(float dt);

#ifndef VC_PLATFORM
#error "Please define VC_PLATFORM macro"
#endif

// Possible values of VC_PLATFORM
#define VC_WASM_PLATFORM 0
#define VC_SDL_PLATFORM 1
#define VC_TERM_PLATFORM 2

#if VC_PLATFORM == VC_SDL_PLATFORM
#include <stdio.h>
#include <SDL2/SDL.h>

#define return_defer(value) do { result = (value); goto defer; } while (0)

static SDL_Texture *vc_sdl_texture = NULL;
static size_t vc_sdl_actual_width = 0;
static size_t vc_sdl_actual_height = 0;

static bool vc_sdl_resize_texture(SDL_Renderer *renderer, size_t new_width, size_t new_height)
{
    if (vc_sdl_texture != NULL) SDL_DestroyTexture(vc_sdl_texture);
    vc_sdl_actual_width = new_width;
    vc_sdl_actual_height = new_height;
    vc_sdl_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, vc_sdl_actual_width, vc_sdl_actual_height);
    if (vc_sdl_texture == NULL) return false;
    return true;
}

int main(void)
{
    int result = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) return_defer(1);

        window = SDL_CreateWindow("Olivec", 0, 0, 0, 0, 0);
        if (window == NULL) return_defer(1);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) return_defer(1);

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
            Olivec_Canvas oc_src = vc_render(dt);
            if (oc_src.width != vc_sdl_actual_width || oc_src.height != vc_sdl_actual_height) {
                if (!vc_sdl_resize_texture(renderer, oc_src.width, oc_src.height)) return_defer(1);
                SDL_SetWindowSize(window, vc_sdl_actual_width, vc_sdl_actual_height);
            }
            void *pixels_dst;
            int pitch;
            SDL_Rect window_rect = {0, 0, vc_sdl_actual_width, vc_sdl_actual_height};
            if (SDL_LockTexture(vc_sdl_texture, &window_rect, &pixels_dst, &pitch) < 0) return_defer(1);
            for (size_t y = 0; y < vc_sdl_actual_height; ++y) {
                // TODO: it would be cool if Olivec_Canvas supported pitch in bytes instead of pixels
                // It would be more flexible and we could draw on the locked texture memory directly
                memcpy((char*)pixels_dst + y*pitch, oc_src.pixels + y*vc_sdl_actual_width, vc_sdl_actual_width*sizeof(uint32_t));
            }
            SDL_UnlockTexture(vc_sdl_texture);

            // Display the texture
            if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0) < 0) return_defer(1);
            if (SDL_RenderClear(renderer) < 0) return_defer(1);
            if (SDL_RenderCopy(renderer, vc_sdl_texture, &window_rect, &window_rect) < 0) return_defer(1);
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
    if (vc_sdl_texture) SDL_DestroyTexture(vc_sdl_texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}
#elif VC_PLATFORM == VC_TERM_PLATFORM

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

static size_t vc_term_actual_width = 0;
static size_t vc_term_actual_height = 0;
static size_t vc_term_scaled_down_width = 0;
static size_t vc_term_scaled_down_height = 0;
static char *vc_term_char_canvas = 0;

// TODO: use ANSI terminal colors for color_to_char
static char vc_term_color_to_char(uint32_t pixel)
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

static uint32_t vc_term_compress_pixels_chunk(Olivec_Canvas oc)
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

#ifndef VC_TERM_SCALE_DOWN_FACTOR
#define VC_TERM_SCALE_DOWN_FACTOR 20
#endif // VC_TERM_SCALE_DOWN_FACTOR

static void vc_term_resize_char_canvas(size_t new_width, size_t new_height)
{
    // TODO: can we just do something so the divisibility is not important?
    // Like round the stuff or something?
    assert(new_width%VC_TERM_SCALE_DOWN_FACTOR == 0 && "Width must be divisible by VC_TERM_SCALE_DOWN_FACTOR");
    assert(new_height%VC_TERM_SCALE_DOWN_FACTOR == 0 && "Height must be divisible by VC_TERM_SCALE_DOWN_FACTOR");
    vc_term_actual_width = new_width;
    vc_term_actual_height = new_height;
    vc_term_scaled_down_width  = vc_term_actual_width/VC_TERM_SCALE_DOWN_FACTOR;
    vc_term_scaled_down_height = vc_term_actual_height/VC_TERM_SCALE_DOWN_FACTOR;
    free(vc_term_char_canvas);
    vc_term_char_canvas = malloc(sizeof(*vc_term_char_canvas)*vc_term_scaled_down_width*vc_term_scaled_down_height);
    assert(vc_term_char_canvas != NULL && "Just buy more RAM");
}

static void vc_term_compress_pixels(Olivec_Canvas oc)
{
    if (vc_term_actual_width != oc.width || vc_term_actual_height != oc.height) {
        vc_term_resize_char_canvas(oc.width, oc.height);
    }

    for (size_t y = 0; y < vc_term_scaled_down_height; ++y) {
        for (size_t x = 0; x < vc_term_scaled_down_width; ++x) {
            Olivec_Canvas soc = olivec_subcanvas(oc, x*VC_TERM_SCALE_DOWN_FACTOR, y*VC_TERM_SCALE_DOWN_FACTOR, VC_TERM_SCALE_DOWN_FACTOR,
VC_TERM_SCALE_DOWN_FACTOR);
            vc_term_char_canvas[y*vc_term_scaled_down_width + x] = vc_term_color_to_char(vc_term_compress_pixels_chunk(soc));
        }
    }
}

int main(void)
{
    for (;;) {
        vc_term_compress_pixels(vc_render(1.f/60.f));
        for (size_t y = 0; y < vc_term_scaled_down_height; ++y) {
            for (size_t x = 0; x < vc_term_scaled_down_width; ++x) {
                // TODO: different halfs of the double pixels
                // We can do stuff like putc('<', stdout); putc('>', stdout);
                putc(vc_term_char_canvas[y*vc_term_scaled_down_width + x], stdout);
                putc(vc_term_char_canvas[y*vc_term_scaled_down_width + x], stdout);
                // TODO: explore the idea of figuring out aspect ratio of the character using escape ANSI codes of the terminal and rendering the image accordingly
            }
            putc('\n', stdout);
        }

        usleep(1000*1000/60);
        printf("\033[%zuA", vc_term_scaled_down_height);
        printf("\033[%zuD", vc_term_scaled_down_width);
    }
    return 0;
}
#elif VC_PLATFORM == VC_WASM_PLATFORM
// Do nothing because all the work is done in ../js/vc.js
#else
#error "Unknown VC platform"
#endif // VC_SDL_PLATFORM
