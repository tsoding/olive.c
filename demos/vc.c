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

int main(int argc, char **args)
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
        bool pause = false;
        for (;;) {
            // Compute Delta Time
            Uint32 curr = SDL_GetTicks();
            float dt = (curr - prev)/1000.f;
            prev = curr;

            // Flush the events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT: {
                    return_defer(0);
                } break;
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_SPACE) pause = !pause;
                } break;
                }
            }

            SDL_Rect window_rect = {0, 0, vc_sdl_actual_width, vc_sdl_actual_height};

            if (!pause) {
                // Render the texture
                Olivec_Canvas oc_src = vc_render(dt);
                if (oc_src.width != vc_sdl_actual_width || oc_src.height != vc_sdl_actual_height) {
                    if (!vc_sdl_resize_texture(renderer, oc_src.width, oc_src.height)) return_defer(1);
                    SDL_SetWindowSize(window, vc_sdl_actual_width, vc_sdl_actual_height);
                }
                void *pixels_dst;
                int pitch;
                if (SDL_LockTexture(vc_sdl_texture, &window_rect, &pixels_dst, &pitch) < 0) return_defer(1);
                for (size_t y = 0; y < vc_sdl_actual_height; ++y) {
                    // TODO: it would be cool if Olivec_Canvas supported pitch in bytes instead of pixels
                    // It would be more flexible and we could draw on the locked texture memory directly
                    memcpy((char*)pixels_dst + y*pitch, oc_src.pixels + y*vc_sdl_actual_width, vc_sdl_actual_width*sizeof(uint32_t));
                }
                SDL_UnlockTexture(vc_sdl_texture);
            }

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
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static size_t vc_term_actual_width = 0;
static size_t vc_term_actual_height = 0;
static size_t vc_term_scaled_down_width = 0;
static size_t vc_term_scaled_down_height = 0;
static int *vc_term_char_canvas = 0;

int hsl256[][3] = {
    {0, 0, 0},
    {0, 100, 25},
    {120, 100, 25},
    {60, 100, 25},
    {240, 100, 25},
    {300, 100, 25},
    {180, 100, 25},
    {0, 0, 75},
    {0, 0, 50},
    {0, 100, 50},
    {120, 100, 50},
    {60, 100, 50},
    {240, 100, 50},
    {300, 100, 50},
    {180, 100, 50},
    {0, 0, 100},
    {0, 0, 0},
    {240, 99, 18},
    {240, 100, 26},
    {240, 100, 34},
    {240, 100, 42},
    {240, 100, 50},
    {120, 99, 18},
    {180, 99, 18},
    {197, 100, 26},
    {207, 100, 34},
    {213, 100, 42},
    {217, 100, 50},
    {120, 100, 26},
    {162, 100, 26},
    {180, 100, 26},
    {193, 100, 34},
    {202, 100, 42},
    {208, 100, 50},
    {120, 100, 34},
    {152, 100, 34},
    {166, 100, 34},
    {180, 100, 34},
    {191, 100, 42},
    {198, 100, 50},
    {120, 100, 42},
    {146, 100, 42},
    {157, 100, 42},
    {168, 100, 42},
    {180, 100, 42},
    {189, 100, 50},
    {120, 100, 50},
    {142, 100, 50},
    {151, 100, 50},
    {161, 100, 50},
    {170, 100, 50},
    {180, 100, 50},
    {0, 99, 18},
    {300, 99, 18},
    {282, 100, 26},
    {272, 100, 34},
    {266, 100, 42},
    {262, 100, 50},
    {60, 99, 18},
    {0, 0, 37},
    {240, 17, 45},
    {240, 33, 52},
    {240, 60, 60},
    {240, 100, 68},
    {77, 100, 26},
    {120, 17, 45},
    {180, 17, 45},
    {210, 33, 52},
    {220, 60, 60},
    {225, 100, 68},
    {87, 100, 34},
    {120, 33, 52},
    {150, 33, 52},
    {180, 33, 52},
    {200, 60, 60},
    {210, 100, 68},
    {93, 100, 42},
    {120, 60, 60},
    {140, 60, 60},
    {160, 60, 60},
    {180, 60, 60},
    {195, 100, 68},
    {97, 100, 50},
    {120, 100, 68},
    {135, 100, 68},
    {150, 100, 68},
    {165, 100, 68},
    {180, 100, 68},
    {0, 100, 26},
    {317, 100, 26},
    {300, 100, 26},
    {286, 100, 34},
    {277, 100, 42},
    {271, 100, 50},
    {42, 100, 26},
    {0, 17, 45},
    {300, 17, 45},
    {270, 33, 52},
    {260, 60, 60},
    {255, 100, 68},
    {60, 100, 26},
    {60, 17, 45},
    {0, 0, 52},
    {240, 20, 60},
    {240, 50, 68},
    {240, 100, 76},
    {73, 100, 34},
    {90, 33, 52},
    {120, 20, 60},
    {180, 20, 60},
    {210, 50, 68},
    {220, 100, 76},
    {82, 100, 42},
    {100, 60, 60},
    {120, 50, 68},
    {150, 50, 68},
    {180, 50, 68},
    {200, 100, 76},
    {88, 100, 50},
    {105, 100, 68},
    {120, 100, 76},
    {140, 100, 76},
    {160, 100, 76},
    {180, 100, 76},
    {0, 100, 34},
    {327, 100, 34},
    {313, 100, 34},
    {300, 100, 34},
    {288, 100, 42},
    {281, 100, 50},
    {32, 100, 34},
    {0, 33, 52},
    {330, 33, 52},
    {300, 33, 52},
    {280, 60, 60},
    {270, 100, 68},
    {46, 100, 34},
    {30, 33, 52},
    {0, 20, 60},
    {300, 20, 60},
    {270, 50, 68},
    {260, 100, 76},
    {60, 100, 34},
    {60, 33, 52},
    {60, 20, 60},
    {0, 0, 68},
    {240, 33, 76},
    {240, 100, 84},
    {71, 100, 42},
    {80, 60, 60},
    {90, 50, 68},
    {120, 33, 76},
    {180, 33, 76},
    {210, 100, 84},
    {78, 100, 50},
    {90, 100, 68},
    {100, 100, 76},
    {120, 100, 84},
    {150, 100, 84},
    {180, 100, 84},
    {0, 100, 42},
    {333, 100, 42},
    {322, 100, 42},
    {311, 100, 42},
    {300, 100, 42},
    {290, 100, 50},
    {26, 100, 42},
    {0, 60, 60},
    {340, 60, 60},
    {320, 60, 60},
    {300, 60, 60},
    {285, 100, 68},
    {37, 100, 42},
    {20, 60, 60},
    {0, 50, 68},
    {330, 50, 68},
    {300, 50, 68},
    {280, 100, 76},
    {48, 100, 42},
    {40, 60, 60},
    {30, 50, 68},
    {0, 33, 76},
    {300, 33, 76},
    {270, 100, 84},
    {60, 100, 42},
    {60, 60, 60},
    {60, 50, 68},
    {60, 33, 76},
    {0, 0, 84},
    {240, 100, 92},
    {69, 100, 50},
    {75, 100, 68},
    {80, 100, 76},
    {90, 100, 84},
    {120, 100, 92},
    {180, 100, 92},
    {0, 100, 50},
    {337, 100, 50},
    {328, 100, 50},
    {318, 100, 50},
    {309, 100, 50},
    {300, 100, 50},
    {22, 100, 50},
    {0, 100, 68},
    {345, 100, 68},
    {330, 100, 68},
    {315, 100, 68},
    {300, 100, 68},
    {31, 100, 50},
    {15, 100, 68},
    {0, 100, 76},
    {340, 100, 76},
    {320, 100, 76},
    {300, 100, 76},
    {41, 100, 50},
    {30, 100, 68},
    {20, 100, 76},
    {0, 100, 84},
    {330, 100, 84},
    {300, 100, 84},
    {50, 100, 50},
    {45, 100, 68},
    {40, 100, 76},
    {30, 100, 84},
    {0, 100, 92},
    {300, 100, 92},
    {60, 100, 50},
    {60, 100, 68},
    {60, 100, 76},
    {60, 100, 84},
    {60, 100, 92},
    {0, 0, 100},
    {0, 0, 3},
    {0, 0, 7},
    {0, 0, 10},
    {0, 0, 14},
    {0, 0, 18},
    {0, 0, 22},
    {0, 0, 26},
    {0, 0, 30},
    {0, 0, 34},
    {0, 0, 38},
    {0, 0, 42},
    {0, 0, 46},
    {0, 0, 50},
    {0, 0, 54},
    {0, 0, 58},
    {0, 0, 61},
    {0, 0, 65},
    {0, 0, 69},
    {0, 0, 73},
    {0, 0, 77},
    {0, 0, 81},
    {0, 0, 85},
    {0, 0, 89},
    {0, 0, 93},
};

int distance_hsl256(int i, int h, int s, int l)
{
    int dh = h - hsl256[i][0];
    int ds = s - hsl256[i][1];
    int dl = l - hsl256[i][2];
    return dh*dh + ds*ds + dl*dl;
}

// TODO: bring find_ansi_index_by_rgb from image2term
int find_ansi_index_by_hsl(int h, int s, int l)
{
    int index = 0;
    for (int i = 0; i < 256; ++i) {
        if (distance_hsl256(i, h, s, l) < distance_hsl256(index, h, s, l)) {
            index = i;
        }
    }
    return index;
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
    // TODO: warn the user if vc_term_actual_width does not fit into the screen
    // TODO: can we just do something so the divisibility is not important?
    // Like round the stuff or something?
    // Or we can resize the frame on the fly similarly to how we resize sprites in olivec_sprite_*() functions.
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

void rgb_to_hsl(int r, int g, int b, int *h, int *s, int *l)
{
    float r01 = r/255.0f;
    float g01 = g/255.0f;
    float b01 = b/255.0f;
    float cmax = r01;
    if (g01 > cmax) cmax = g01;
    if (b01 > cmax) cmax = b01;
    float cmin = r01;
    if (g01 < cmin) cmin = g01;
    if (b01 < cmin) cmin = b01;
    float delta = cmax - cmin;
    float epsilon = 1e-6;
    float hf = 0;
    if (delta < epsilon) hf = 0;
    else if (cmax == r01) hf = 60.0f*fmod((g01 - b01)/delta, 6.0f);
    else if (cmax == g01) hf = 60.0f*((b01 - r01)/delta + 2);
    else if (cmax == b01) hf = 60.0f*((r01 - g01)/delta + 4);
    else assert(0 && "unreachable");

    float lf = (cmax + cmin)/2;

    float sf = 0;
    if (delta < epsilon) sf = 0;
    else sf = delta/(1 - fabsf(2*lf - 1));

    *h = fmodf(fmodf(hf, 360.0f) + 360.0f, 360.0f);
    *s = sf*100.0f;
    *l = lf*100.0f;
}

static void vc_term_compress_pixels(Olivec_Canvas oc)
{
    if (vc_term_actual_width != oc.width || vc_term_actual_height != oc.height) {
        vc_term_resize_char_canvas(oc.width, oc.height);
    }

    for (size_t y = 0; y < vc_term_scaled_down_height; ++y) {
        for (size_t x = 0; x < vc_term_scaled_down_width; ++x) {
            Olivec_Canvas soc = olivec_subcanvas(oc, x*VC_TERM_SCALE_DOWN_FACTOR, y*VC_TERM_SCALE_DOWN_FACTOR, VC_TERM_SCALE_DOWN_FACTOR, VC_TERM_SCALE_DOWN_FACTOR);
            uint32_t cp = vc_term_compress_pixels_chunk(soc);
            int r = OLIVEC_RED(cp);
            int g = OLIVEC_GREEN(cp);
            int b = OLIVEC_BLUE(cp);
            int a = OLIVEC_ALPHA(cp);
            r = a*r/255;
            g = a*g/255;
            b = a*b/255;
            int h, s, l;
            rgb_to_hsl(r, g, b, &h, &s, &l);
            vc_term_char_canvas[y*vc_term_scaled_down_width + x] = find_ansi_index_by_hsl(h, s, l);
        }
    }
}

int main(void)
{
    for (;;) {
        vc_term_compress_pixels(vc_render(1.f/60.f));
        for (size_t y = 0; y < vc_term_scaled_down_height; ++y) {
            for (size_t x = 0; x < vc_term_scaled_down_width; ++x) {
                // TODO: explore the idea of figuring out aspect ratio of the character using escape ANSI codes of the terminal and rendering the image accordingly
                printf("\033[48;5;%dm  ", vc_term_char_canvas[y*vc_term_scaled_down_width + x]);
            }
            printf("\033[0m\n");
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
