#ifndef OLIVE_C_
#define OLIVE_C_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef OLIVECDEF
#define OLIVECDEF static inline
#endif

#ifndef OLIVEC_AA_RES
#define OLIVEC_AA_RES 2
#endif

#define OLIVEC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x)*(x))

typedef struct {
    size_t width, height;
    const char *glyphs;
} Olivec_Font;

#define DEFAULT_FONT_HEIGHT 6
#define DEFAULT_FONT_WIDTH 6
// TODO: allocate proper descender and acender areas for the default font
static char default_font_glyphs[128][DEFAULT_FONT_HEIGHT][DEFAULT_FONT_WIDTH] = {
    ['a'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
    },
    ['b'] = {
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
    },
    ['c'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['d'] = {
        {0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
    },
    ['e'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
    },
    ['f'] = {
        {0, 0, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    ['g'] = {},
    ['h'] = {
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
    },
    ['i'] = {},
    ['j'] = {},
    ['k'] = {},
    ['l'] = {
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
    },
    ['m'] = {},
    ['n'] = {},
    ['o'] = {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['p'] = {
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    ['q'] = {},
    ['r'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    ['s'] = {},
    ['t'] = {},
    ['u'] = {},
    ['v'] = {},
    ['w'] = {
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {0, 1, 1, 1, 1},
    },
    ['x'] = {},
    ['y'] = {},
    ['z'] = {},

    ['A'] = {},
    ['B'] = {},
    ['C'] = {},
    ['D'] = {},
    ['E'] = {},
    ['F'] = {},
    ['G'] = {},
    ['H'] = {},
    ['I'] = {},
    ['J'] = {},
    ['K'] = {},
    ['L'] = {},
    ['M'] = {},
    ['N'] = {},
    ['O'] = {},
    ['P'] = {},
    ['Q'] = {},
    ['R'] = {},
    ['S'] = {},
    ['T'] = {},
    ['U'] = {},
    ['V'] = {},
    ['W'] = {},
    ['X'] = {},
    ['Y'] = {},
    ['Z'] = {},

    ['0'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['1'] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
    },
    ['2'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
    },
    ['3'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['4'] = {
        {0, 0, 1, 1, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
    },
    ['5'] = {
        {1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['6'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },
    ['7'] = {
        {1, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
    },
    ['8'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},

    },
    ['9'] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
    },

    [','] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },

    ['.'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
    },
    ['-'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
};

static Olivec_Font default_font = {
    .glyphs = &default_font_glyphs[0][0][0],
    .width = DEFAULT_FONT_WIDTH,
    .height = DEFAULT_FONT_HEIGHT,
};

typedef struct {
    uint32_t *pixels;
    size_t width;
    size_t height;
    size_t stride;
} Olivec_Canvas;

#define OLIVEC_CANVAS_NULL ((Olivec_Canvas) {0})
#define OLIVEC_PIXEL(oc, x, y) (oc).pixels[(y)*(oc).stride + (x)]

OLIVECDEF Olivec_Canvas olivec_canvas(uint32_t *pixels, size_t width, size_t height, size_t stride);
OLIVECDEF Olivec_Canvas olivec_subcanvas(Olivec_Canvas oc, int x, int y, int w, int h);
OLIVECDEF void olivec_blend_color(uint32_t *c1, uint32_t c2);
OLIVECDEF void olivec_fill(Olivec_Canvas oc, uint32_t color);
OLIVECDEF void olivec_rect(Olivec_Canvas oc, int x, int y, int w, int h, uint32_t color);
OLIVECDEF void olivec_frame(Olivec_Canvas oc, int x, int y, int w, int h, size_t thiccness, uint32_t color);
OLIVECDEF void olivec_circle(Olivec_Canvas oc, int cx, int cy, int r, uint32_t color);
// TODO: lines with different thiccness
OLIVECDEF void olivec_line(Olivec_Canvas oc, int x1, int y1, int x2, int y2, uint32_t color);
OLIVECDEF void olivec_triangle(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
OLIVECDEF void olivec_triangle3(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t c1, uint32_t c2, uint32_t c3);
OLIVECDEF void olivec_text(Olivec_Canvas oc, const char *text, int x, int y, Olivec_Font font, size_t size, uint32_t color);

// The point of this function is to produce two ranges x1..x2 and y1..y2 that are guaranteed to be safe to iterate over the canvas of size pixels_width by pixels_height without any boundary checks.
//
// if (olivec_normalize_rect(x, y, w, h, WIDTH, HEIGHT, &x1, &y1, &x2, &y2)) {
//     for (int x = x1; x <= x2; ++x) {
//         for (int y = y1; y <= y2; ++y) {
//             OLIVEC_PIXEL(oc, x, y) = 0x69696969;
//         }
//     }
// } else {
//     // Rectangle is invisible cause it's completely out-of-bounds
// }
OLIVECDEF bool olivec_normalize_rect(int x, int y, int w, int h, size_t pixels_width, size_t pixels_height, int *x1, int *x2, int *y1, int *y2);

#endif // OLIVE_C_

#ifdef OLIVEC_IMPLEMENTATION

OLIVECDEF Olivec_Canvas olivec_canvas(uint32_t *pixels, size_t width, size_t height, size_t stride)
{
    Olivec_Canvas oc = {
        .pixels = pixels,
        .width  = width,
        .height = height,
        .stride = stride,
    };
    return oc;
}

OLIVECDEF bool olivec_normalize_rect(int x, int y, int w, int h,
                                     size_t pixels_width, size_t pixels_height,
                                     int *x1, int *x2, int *y1, int *y2)
{
    // No need to render empty rectangle
    if (w == 0) return false;
    if (h == 0) return false;

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

OLIVECDEF Olivec_Canvas olivec_subcanvas(Olivec_Canvas oc, int x, int y, int w, int h)
{
    int x1, x2, y1, y2;
    if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &x1, &x2, &y1, &y2)) return OLIVEC_CANVAS_NULL;
    oc.pixels = &OLIVEC_PIXEL(oc, x1, y1);
    oc.width = x2 - x1 + 1;
    oc.height = y2 - y1 + 1;
    return oc;
}

// TODO: custom pixel formats
// Maybe we can store pixel format info in Olivec_Canvas
#define OLIVEC_RED(color)   (((color)&0x000000FF)>>(8*0))
#define OLIVEC_GREEN(color) (((color)&0x0000FF00)>>(8*1))
#define OLIVEC_BLUE(color)  (((color)&0x00FF0000)>>(8*2))
#define OLIVEC_ALPHA(color) (((color)&0xFF000000)>>(8*3))
#define OLIVEC_RGBA(r, g, b, a) ((((r)&0xFF)<<(8*0)) | (((g)&0xFF)<<(8*1)) | (((b)&0xFF)<<(8*2)) | (((a)&0xFF)<<(8*3)))

OLIVECDEF void olivec_blend_color(uint32_t *c1, uint32_t c2)
{
    uint32_t r1 = OLIVEC_RED(*c1);
    uint32_t g1 = OLIVEC_GREEN(*c1);
    uint32_t b1 = OLIVEC_BLUE(*c1);
    uint32_t a1 = OLIVEC_ALPHA(*c1);

    uint32_t r2 = OLIVEC_RED(c2);
    uint32_t g2 = OLIVEC_GREEN(c2);
    uint32_t b2 = OLIVEC_BLUE(c2);
    uint32_t a2 = OLIVEC_ALPHA(c2);

    r1 = (r1*(255 - a2) + r2*a2)/255; if (r1 > 255) r1 = 255;
    g1 = (g1*(255 - a2) + g2*a2)/255; if (g1 > 255) g1 = 255;
    b1 = (b1*(255 - a2) + b2*a2)/255; if (b1 > 255) b1 = 255;

    *c1 = OLIVEC_RGBA(r1, g1, b1, a1);
}

OLIVECDEF void olivec_fill(Olivec_Canvas oc, uint32_t color)
{
    for (size_t y = 0; y < oc.height; ++y) {
        for (size_t x = 0; x < oc.width; ++x) {
            OLIVEC_PIXEL(oc, x, y) = color;
        }
    }
}

OLIVECDEF void olivec_rect(Olivec_Canvas oc, int x, int y, int w, int h, uint32_t color)
{
    int x1, x2, y1, y2;
    if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &x1, &x2, &y1, &y2)) return;
    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
        }
    }
}

OLIVECDEF void olivec_frame(Olivec_Canvas oc, int x, int y, int w, int h, size_t t, uint32_t color)
{
    if (t == 0) return; // Nothing to render

    // Convert the rectangle to 2-points representation
    int x1 = x;
    int y1 = y;
    int x2 = x1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);
    if (x1 > x2) OLIVEC_SWAP(int, x1, x2);
    int y2 = y1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
    if (y1 > y2) OLIVEC_SWAP(int, y1, y2);

    olivec_rect(oc, x1 - t/2, y1 - t/2, (x2 - x1 + 1) + t/2*2, t, color);  // Top
    olivec_rect(oc, x1 - t/2, y1 - t/2, t, (y2 - y1 + 1) + t/2*2, color);  // Left
    olivec_rect(oc, x1 - t/2, y2 + t/2, (x2 - x1 + 1) + t/2*2, -t, color); // Bottom
    olivec_rect(oc, x2 + t/2, y1 - t/2, -t, (y2 - y1 + 1) + t/2*2, color); // Right
}

OLIVECDEF void olivec_circle(Olivec_Canvas oc, int cx, int cy, int r, uint32_t color)
{
    int x1, y1, x2, y2;
    int r1 = r + OLIVEC_SIGN(int, r);
    if (!olivec_normalize_rect(cx - r1, cy - r1, 2*r1, 2*r1, oc.width, oc.height, &x1, &x2, &y1, &y2)) return;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            int count = 0;
            for (int sox = 0; sox < OLIVEC_AA_RES; ++sox) {
                for (int soy = 0; soy < OLIVEC_AA_RES; ++soy) {
                    // TODO: switch to 64 bits to make the overflow less likely
                    // Also research the probability of overflow
                    int res1 = (OLIVEC_AA_RES + 1);
                    int dx = (x*res1*2 + 2 + sox*2 - res1*cx*2 - res1);
                    int dy = (y*res1*2 + 2 + soy*2 - res1*cy*2 - res1);
                    if (dx*dx + dy*dy <= res1*res1*r*r*2*2) count += 1;
                }
            }
            uint32_t alpha = ((color&0xFF000000)>>(3*8))*count/OLIVEC_AA_RES/OLIVEC_AA_RES;
            uint32_t updated_color = (color&0x00FFFFFF)|(alpha<<(3*8));
            olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), updated_color);
        }
    }
}

// TODO: AA for line
OLIVECDEF void olivec_line(Olivec_Canvas oc, int x1, int y1, int x2, int y2, uint32_t color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    // If both of the differences are 0 there will be a division by 0 below.
    if (dx == 0 && dy == 0) {
        if (0 <= x1 && x1 < (int) oc.width && 0 <= y1 && y1 < (int) oc.height) {
            olivec_blend_color(&OLIVEC_PIXEL(oc, x1, y1), color);
        }
        return;
    }

    if (OLIVEC_ABS(int, dx) > OLIVEC_ABS(int, dy)) {
        if (x1 > x2) {
            OLIVEC_SWAP(int, x1, x2);
            OLIVEC_SWAP(int, y1, y2);
        }

        // Cull out invisible line
        if (x1 > (int) oc.width) return;
        if (x2 < 0) return;

        // Clamp the line to the boundaries
        if (x1 < 0) x1 = 0;
        if (x2 >= (int) oc.width) x2 = (int) oc.width - 1;

        for (int x = x1; x <= x2; ++x) {
            int y = dy*(x - x1)/dx + y1;
            // TODO: move boundary checks out side of the loops in olivec_draw_line
            if (0 <= y && y < (int) oc.height) {
                olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
            }
        }
    } else {
        if (y1 > y2) {
            OLIVEC_SWAP(int, x1, x2);
            OLIVEC_SWAP(int, y1, y2);
        }

        // Cull out invisible line
        if (y1 > (int) oc.height) return;
        if (y2 < 0) return;

        // Clamp the line to the boundaries
        if (y1 < 0) y1 = 0;
        if (y2 >= (int) oc.height) y2 = (int) oc.height - 1;

        for (int y = y1; y <= y2; ++y) {
            int x = dx*(y - y1)/dy + x1;
            // TODO: move boundary checks out side of the loops in olivec_draw_line
            if (0 <= x && x < (int) oc.width) {
                olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
            }
        }
    }
}

OLIVECDEF uint32_t mix_colors3(uint32_t c1, uint32_t c2, uint32_t c3, int t1, int t2, int t3, int den)
{
    // TODO: estimate how much overflows are an issue in integer only environment
    int64_t r1 = OLIVEC_RED(c1);
    int64_t g1 = OLIVEC_GREEN(c1);
    int64_t b1 = OLIVEC_BLUE(c1);
    int64_t a1 = OLIVEC_ALPHA(c1);

    int64_t r2 = OLIVEC_RED(c2);
    int64_t g2 = OLIVEC_GREEN(c2);
    int64_t b2 = OLIVEC_BLUE(c2);
    int64_t a2 = OLIVEC_ALPHA(c2);

    int64_t r3 = OLIVEC_RED(c3);
    int64_t g3 = OLIVEC_GREEN(c3);
    int64_t b3 = OLIVEC_BLUE(c3);
    int64_t a3 = OLIVEC_ALPHA(c3);

    if (den != 0) {
        int64_t r4 = (r1*t1 + r2*t2 + r3*t3)/den;
        int64_t g4 = (g1*t1 + g2*t2 + g3*t3)/den;
        int64_t b4 = (b1*t1 + b2*t2 + b3*t3)/den;
        int64_t a4 = (a1*t1 + a2*t2 + a3*t3)/den;

        return OLIVEC_RGBA(r4, g4, b4, a4);
    }

    return 0;
}

OLIVECDEF void barycentric(int x1, int y1, int x2, int y2, int x3, int y3,
                           int xp, int yp,
                           int *u1, int *u2, int *det)
{
    *det = ((x1 - x3)*(y2 - y3) - (x2 - x3)*(y1 - y3));
    *u1  = ((y2 - y3)*(xp - x3) + (x3 - x2)*(yp - y3));
    *u2  = ((y3 - y1)*(xp - x3) + (x1 - x3)*(yp - y3));
    // u3 = det - u1 - u2
}

OLIVECDEF void olivec_triangle3(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3,
                                uint32_t c1, uint32_t c2, uint32_t c3)
{
    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
        OLIVEC_SWAP(int, c1, c2);
    }

    if (y2 > y3) {
        OLIVEC_SWAP(int, x2, x3);
        OLIVEC_SWAP(int, y2, y3);
        OLIVEC_SWAP(int, c2, c3);
    }

    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
        OLIVEC_SWAP(int, c1, c2);
    }

    int dx12 = x2 - x1;
    int dy12 = y2 - y1;
    int dx13 = x3 - x1;
    int dy13 = y3 - y1;

    for (int y = y1; y <= y2; ++y) {
        // TODO: move boundary checks outside of loops in olivec_fill_triangle
        if (0 <= y && (size_t) y < oc.height) {
            int s1 = dy12 != 0 ? (y - y1)*dx12/dy12 + x1 : x1;
            int s2 = dy13 != 0 ? (y - y1)*dx13/dy13 + x1 : x1;
            if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
            for (int x = s1; x <= s2; ++x) {
                if (0 <= x && (size_t) x < oc.width) {
                    int u1, u2, det;
                    barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det);
                    uint32_t color = mix_colors3(c1, c2, c3, u1, u2, det - u1 - u2, det);
                    olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
                }
            }
        }
    }

    int dx32 = x2 - x3;
    int dy32 = y2 - y3;
    int dx31 = x1 - x3;
    int dy31 = y1 - y3;

    for (int y = y2; y <= y3; ++y) {
        if (0 <= y && (size_t) y < oc.height) {
            int s1 = dy32 != 0 ? (y - y3)*dx32/dy32 + x3 : x3;
            int s2 = dy31 != 0 ? (y - y3)*dx31/dy31 + x3 : x3;
            if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
            for (int x = s1; x <= s2; ++x) {
                if (0 <= x && (size_t) x < oc.width) {
                    int u1, u2, det;
                    barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det);
                    uint32_t color = mix_colors3(c1, c2, c3, u1, u2, det - u1 - u2, det);
                    olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
                }
            }
        }
    }
}

// TODO: AA for triangle
OLIVECDEF void olivec_triangle(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
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
        if (0 <= y && (size_t) y < oc.height) {
            int s1 = dy12 != 0 ? (y - y1)*dx12/dy12 + x1 : x1;
            int s2 = dy13 != 0 ? (y - y1)*dx13/dy13 + x1 : x1;
            if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
            for (int x = s1; x <= s2; ++x) {
                if (0 <= x && (size_t) x < oc.width) {
                    olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
                }
            }
        }
    }

    int dx32 = x2 - x3;
    int dy32 = y2 - y3;
    int dx31 = x1 - x3;
    int dy31 = y1 - y3;

    for (int y = y2; y <= y3; ++y) {
        if (0 <= y && (size_t) y < oc.height) {
            int s1 = dy32 != 0 ? (y - y3)*dx32/dy32 + x3 : x3;
            int s2 = dy31 != 0 ? (y - y3)*dx31/dy31 + x3 : x3;
            if (s1 > s2) OLIVEC_SWAP(int, s1, s2);
            for (int x = s1; x <= s2; ++x) {
                if (0 <= x && (size_t) x < oc.width) {
                    olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), color);
                }
            }
        }
    }
}

OLIVECDEF void olivec_text(Olivec_Canvas oc, const char *text, int tx, int ty, Olivec_Font font, size_t glyph_size, uint32_t color)
{
    for (size_t i = 0; *text; ++i, ++text) {
        int gx = tx + i*font.width*glyph_size;
        int gy = ty;
        const char *glyph = &font.glyphs[(*text)*sizeof(char)*font.width*font.height];
        for (int dy = 0; (size_t) dy < font.height; ++dy) {
            for (int dx = 0; (size_t) dx < font.width; ++dx) {
                int px = gx + dx*glyph_size;
                int py = gy + dy*glyph_size;
                if (0 <= px && px < (int) oc.width && 0 <= py && py < (int) oc.height) {
                    if (glyph[dy*font.width + dx]) {
                        olivec_rect(oc, px, py, glyph_size, glyph_size, color);
                    }
                }
            }
        }
    }
}

// TODO: bilinear interpolation for olivec_copy
OLIVECDEF void olivec_copy(Olivec_Canvas src, Olivec_Canvas dst, int x, int y, int w, int h)
{
    int ox1 = x;
    int oy1 = y;

    // Convert the rectangle to 2-points representation
    int ox2 = ox1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);
    if (ox1 > ox2) OLIVEC_SWAP(int, ox1, ox2);
    int oy2 = oy1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
    if (oy1 > oy2) OLIVEC_SWAP(int, oy1, oy2);

    // Cull out invisible rectangle
    if (ox1 >= (int) dst.width) return;
    if (ox2 < 0) return;
    if (oy1 >= (int) dst.height) return;
    if (oy2 < 0) return;

    // Clamp the rectangle to the boundaries
    int x1 = ox1, x2 = ox2, y1 = oy1, y2 = oy2;
    if (x1 < 0) x1 = 0;
    if (x2 >= (int) dst.width) x2 = (int) dst.width - 1;
    if (y1 < 0) y1 = 0;
    if (y2 >= (int) dst.height) y2 = (int) dst.height - 1;

    int xa = ox1; if (w < 0) xa = ox2;
    int ya = oy1; if (h < 0) ya = oy2;
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            size_t nx = (x - xa)*((int) src.width)/w;
            size_t ny = (y - ya)*((int) src.height)/h;
            olivec_blend_color(&OLIVEC_PIXEL(dst, x, y), OLIVEC_PIXEL(src, nx, ny));
        }
    }
}

#endif // OLIVEC_IMPLEMENTATION

// TODO: 3D triangles with z-buffering
// TODO: 3D textures
// TODO: Stencil
// TODO: Benchmarking
// TODO: SIMD implementations
// TODO: olivec_ring
// TODO: olivec_ellipse
// TODO: bezier curves
