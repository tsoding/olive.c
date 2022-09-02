#ifndef OLIVE_C_
#define OLIVE_C_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define OLIVEC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x)*(x))

void olivec_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{
    for (size_t i = 0; i < width*height; ++i) {
        pixels[i] = color;
    }
}

// The point of this function is to produce two ranges x1..x2 and y1..y2 that are guaranteed to be safe to iterate over the canvas of size pixels_width by pixels_height without any boundary checks.
// 
// if (olivec_normalize_rect(x, y, w, h, WIDTH, HEIGHT, &x1, &y1, &x2, &y2)) {
//     for (int x = x1; x <= x2; ++x) {
//         for (itn y = y1; y <= y2; ++y) {
//             pixels[y*pixels_width + x] = 0x69696969;
//         }
//     }
// } else {
//     // Rectangle is invisible cause it's completely out-of-bounds
// }
bool olivec_normalize_rect(int x, int y, int w, int h,
                           size_t pixels_width, size_t pixels_height,
                           int *x1, int *x2, int *y1, int *y2)
{
    *x1 = x;
    *y1 = y;

    // Convert the rectangle to 2-points representation
    *x2 = *x1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);
    if (*x1 > *x2) OLIVEC_SWAP(int, *x1, *x2);
    *y2 = *y1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
    if (*y1 > *y2) OLIVEC_SWAP(int, *y1, *y2);

    // Cull out invisible rectangle
    if (*x1 >= (int) pixels_width) return false;
    if (*x2 < 0) return false;
    if (*y1 >= (int) pixels_height) return false;
    if (*y2 < 0) return false;

    // Clamp the rectangle to the boundaries
    if (*x1 < 0) *x1 = 0;
    if (*x2 >= (int) pixels_width) *x2 = (int) pixels_width - 1;
    if (*y1 < 0) *y1 = 0;
    if (*y2 >= (int) pixels_height) *y2 = (int) pixels_height - 1;

    return true;
}

typedef enum {
    COMP_RED = 0,
    COMP_GREEN,
    COMP_BLUE,
    COMP_ALPHA,
    COUNT_COMPS,
} Comp_Index;

void unpack_rgba32(uint32_t c, uint8_t comp[COUNT_COMPS])
{
    for (size_t i = 0; i < COUNT_COMPS; ++i) {
        comp[i] = c&0xFF;
        c >>= 8;
    }
}

uint32_t pack_rgba32(uint8_t comp[COUNT_COMPS])
{
    uint32_t result = 0;
    for (size_t i = 0; i < COUNT_COMPS; ++i) {
        result |= comp[i]<<(8*i);
    }
    return result;
}

uint8_t olivec_mix_comps(uint16_t c1, uint16_t c2, uint16_t a)
{
    return c1 + (c2 - c1)*a/255;
}

uint32_t olivec_mix_colors(uint32_t c1, uint32_t c2)
{
    uint8_t comp1[COUNT_COMPS];
    unpack_rgba32(c1, comp1);

    uint8_t comp2[COUNT_COMPS];
    unpack_rgba32(c2, comp2);

    for (size_t i = 0; i < COMP_ALPHA; ++i) {
        comp1[i] = olivec_mix_comps(comp1[i], comp2[i], comp2[COMP_ALPHA]);
    }

    return pack_rgba32(comp1);
}

void olivec_fill_rect(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x, int y, int w, int h,
                      uint32_t color)
{
    int x1, y1, x2, y2;
    if (!olivec_normalize_rect(x, y, w, h, pixels_width, pixels_height, &x1, &x2, &y1, &y2)) return;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            pixels[y*pixels_width + x] = olivec_mix_colors(pixels[y*pixels_width + x], color);
        }
    }
}

void olivec_fill_circle(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                        int cx, int cy, int r,
                        uint32_t color)
{
    int x1, y1, x2, y2;
    int r1 = r + OLIVEC_SIGN(int, r);
    if (!olivec_normalize_rect(cx - r1, cy - r1, 2*r1, 2*r1, pixels_width, pixels_height, &x1, &x2, &y1, &y2)) return;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx*dx + dy*dy <= r*r) {
                pixels[y*pixels_width + x] = olivec_mix_colors(pixels[y*pixels_width + x], color);
            }
        }
    }
}

// TODO: lines with different thicness
void olivec_draw_line(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x1, int y1, int x2, int y2,
                      uint32_t color)
{
    // TODO: fix the olivec_draw_line stairs
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx != 0) {
        int c = y1 - dy*x1/dx;

        if (x1 > x2) OLIVEC_SWAP(int, x1, x2);
        for (int x = x1; x <= x2; ++x) {
            // TODO: move boundary checks out side of the loops in olivec_draw_line
            if (0 <= x && x < (int) pixels_width) {
                int sy1 = dy*x/dx + c;
                int sy2 = dy*(x + 1)/dx + c;
                if (sy1 > sy2) OLIVEC_SWAP(int, sy1, sy2);
                for (int y = sy1; y <= sy2; ++y) {
                    if (0 <= y && y < (int) pixels_height) {
                        pixels[y*pixels_width + x] = olivec_mix_colors(pixels[y*pixels_width + x], color);
                    }
                }
            }
        }
    } else {
        int x = x1;
        if (0 <= x && x < (int) pixels_width) {
            if (y1 > y2) OLIVEC_SWAP(int, y1, y2);
            for (int y = y1; y <= y2; ++y) {
                if (0 <= y && y < (int) pixels_height) {
                    pixels[y*pixels_width + x] = olivec_mix_colors(pixels[y*pixels_width + x], color);
                }
            }
        }
    }
}

void olivec_fill_triangle(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                          int x1, int y1,
                          int x2, int y2,
                          int x3, int y3,
                          uint32_t color)
{
    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
    }

    if (y2 > y3) {
        OLIVEC_SWAP(int, x2, x3);
        OLIVEC_SWAP(int, y2, y3);
    }

    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
    }

    int dx12 = x2 - x1;
    int dy12 = y2 - y1;
    int dx13 = x3 - x1;
    int dy13 = y3 - y1;

    for (int y = y1; y <= y2; ++y) {
        // TODO: move boundary checks outside of loops in olivec_fill_triangle
        if (0 <= y && (size_t) y < pixels_height) {
            int s1 = dy12 != 0 ? (y - y1)*dx12/dy12 + x1 : x1;
            int s2 = dy13 != 0 ? (y - y1)*dx13/dy13 + x1 : x1;
            if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
            for (int x = s1; x <= s2; ++x) {
                if (0 <= x && (size_t) x < pixels_width) {
                    pixels[y*pixels_width + x] = olivec_mix_colors(pixels[y*pixels_width + x], color);
                }
            }
        }
    }

    int dx32 = x2 - x3;
    int dy32 = y2 - y3;
    int dx31 = x1 - x3;
    int dy31 = y1 - y3;

    for (int y = y2; y <= y3; ++y) {
        if (0 <= y && (size_t) y < pixels_height) {
            int s1 = dy32 != 0 ? (y - y3)*dx32/dy32 + x3 : x3;
            int s2 = dy31 != 0 ? (y - y3)*dx31/dy31 + x3 : x3;
            if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
            for (int x = s1; x <= s2; ++x) {
                if (0 <= x && (size_t) x < pixels_width) {
                    pixels[y*pixels_width + x] = olivec_mix_colors(pixels[y*pixels_width + x], color);
                }
            }
        }
    }
}

// TODO: Olivec_Canvas
// TODO: supersampling
// TODO: olivec_draw_circle
// TODO: olivec_(draw|fill)_ellipse
// TODO: Benchmarking

#endif // OLIVE_C_
