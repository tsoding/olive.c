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
    ['i'] = {
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
    },
    ['j'] = {},
    ['k'] = {
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 0, 1, 0},
    },
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

typedef struct {
    float u;
    float v;
} Uv;

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
OLIVECDEF void olivec_triangle3c(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t c1, uint32_t c2, uint32_t c3);
OLIVECDEF void olivec_triangle3z(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, float z1, float z2, float z3);
OLIVECDEF void olivec_triangle3uv(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, Uv uv1, Uv uv2, Uv uv3, float z1, float z2, float z3, Olivec_Canvas texture);
OLIVECDEF void olivec_text(Olivec_Canvas oc, const char *text, int x, int y, Olivec_Font font, size_t size, uint32_t color);
OLIVECDEF void olivec_sprite_blend(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite);
OLIVECDEF void olivec_sprite_copy(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite);
OLIVECDEF Uv olivec_uv(float u, float v);

typedef struct {
    // Safe ranges to iterate over.
    int x1, x2;
    int y1, y2;

    // Original uncut ranges some parts of which may be outside of the canvas boundaries.
    int ox1, ox2;
    int oy1, oy2;
} Olivec_Normalized_Rect;

// The point of this function is to produce two ranges x1..x2 and y1..y2 that are guaranteed to be safe to iterate over the canvas of size pixels_width by pixels_height without any boundary checks.
//
// Olivec_Normalized_Rect nr = {0};
// if (olivec_normalize_rect(x, y, w, h, WIDTH, HEIGHT, &nr)) {
//     for (int x = nr.x1; x <= nr.x2; ++x) {
//         for (int y = nr.y1; y <= nr.y2; ++y) {
//             OLIVEC_PIXEL(oc, x, y) = 0x69696969;
//         }
//     }
// } else {
//     // Rectangle is invisible cause it's completely out-of-bounds
// }
OLIVECDEF bool olivec_normalize_rect(int x, int y, int w, int h,
                                     size_t canvas_width, size_t canvas_height,
                                     Olivec_Normalized_Rect *nr);

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
                                     size_t canvas_width, size_t canvas_height,
                                     Olivec_Normalized_Rect *nr)
{
    // No need to render empty rectangle
    if (w == 0) return false;
    if (h == 0) return false;

    nr->ox1 = x;
    nr->oy1 = y;

    // Convert the rectangle to 2-points representation
    nr->ox2 = nr->ox1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);
    if (nr->ox1 > nr->ox2) OLIVEC_SWAP(int, nr->ox1, nr->ox2);
    nr->oy2 = nr->oy1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
    if (nr->oy1 > nr->oy2) OLIVEC_SWAP(int, nr->oy1, nr->oy2);

    // Cull out invisible rectangle
    if (nr->ox1 >= (int) canvas_width) return false;
    if (nr->ox2 < 0) return false;
    if (nr->oy1 >= (int) canvas_height) return false;
    if (nr->oy2 < 0) return false;

    nr->x1 = nr->ox1;
    nr->y1 = nr->oy1;
    nr->x2 = nr->ox2;
    nr->y2 = nr->oy2;

    // Clamp the rectangle to the boundaries
    if (nr->x1 < 0) nr->x1 = 0;
    if (nr->x2 >= (int) canvas_width) nr->x2 = (int) canvas_width - 1;
    if (nr->y1 < 0) nr->y1 = 0;
    if (nr->y2 >= (int) canvas_height) nr->y2 = (int) canvas_height - 1;

    return true;
}

OLIVECDEF Olivec_Canvas olivec_subcanvas(Olivec_Canvas oc, int x, int y, int w, int h)
{
    Olivec_Normalized_Rect nr = {0};
    if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &nr)) return OLIVEC_CANVAS_NULL;
    oc.pixels = &OLIVEC_PIXEL(oc, nr.x1, nr.y1);
    oc.width = nr.x2 - nr.x1 + 1;
    oc.height = nr.y2 - nr.y1 + 1;
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
    Olivec_Normalized_Rect nr = {0};
    if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &nr)) return;
    for (int x = nr.x1; x <= nr.x2; ++x) {
        for (int y = nr.y1; y <= nr.y2; ++y) {
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
    Olivec_Normalized_Rect nr = {0};
    int r1 = r + OLIVEC_SIGN(int, r);
    if (!olivec_normalize_rect(cx - r1, cy - r1, 2*r1, 2*r1, oc.width, oc.height, &nr)) return;

    for (int y = nr.y1; y <= nr.y2; ++y) {
        for (int x = nr.x1; x <= nr.x2; ++x) {
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

OLIVECDEF void olivec_triangle3c(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3,
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

OLIVECDEF void olivec_triangle3z(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, float z1, float z2, float z3)
{
    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
        OLIVEC_SWAP(float, z1, z2);
    }

    if (y2 > y3) {
        OLIVEC_SWAP(int, x2, x3);
        OLIVEC_SWAP(int, y2, y3);
        OLIVEC_SWAP(float, z2, z3);
    }

    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
        OLIVEC_SWAP(float, z1, z2);
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
                    float z = z1*u1/det + z2*u2/det + z3*(det - u1 - u2)/det;
                    OLIVEC_PIXEL(oc, x, y) = *(uint32_t*)&z;
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
                    float z = z1*u1/det + z2*u2/det + z3*(det - u1 - u2)/det;
                    OLIVEC_PIXEL(oc, x, y) = *(uint32_t*)&z;
                }
            }
        }
    }
}

OLIVECDEF void olivec_triangle3uv(Olivec_Canvas oc, int x1, int y1, int x2, int y2, int x3, int y3, Uv uv1, Uv uv2, Uv uv3, float z1, float z2, float z3, Olivec_Canvas texture)
{
    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
        OLIVEC_SWAP(Uv, uv1, uv2);
        OLIVEC_SWAP(float, z1, z2);
    }

    if (y2 > y3) {
        OLIVEC_SWAP(int, x2, x3);
        OLIVEC_SWAP(int, y2, y3);
        OLIVEC_SWAP(Uv, uv2, uv3);
        OLIVEC_SWAP(float, z2, z3);
    }

    if (y1 > y2) {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
        OLIVEC_SWAP(Uv, uv1, uv2);
        OLIVEC_SWAP(float, z1, z2);
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
                    int u3 = det - u1 - u2;
                    float z = z1*u1/det + z2*u2/det + z3*(det - u1 - u2)/det;
                    Uv uv = olivec_uv(
                        uv1.u*u1/det + uv2.u*u2/det + uv3.u*u3/det,
                        uv1.v*u1/det + uv2.v*u2/det + uv3.v*u3/det
                    );
                    int texture_x = uv.u/z*texture.width;
                    if (texture_x < 0) texture_x = 0;
                    if ((size_t) texture_x >= texture.width) texture_x = texture.width - 1;
                    int texture_y = uv.v/z*texture.height;
                    if (texture_y < 0) texture_y = 0;
                    if ((size_t) texture_y >= texture.height) texture_y = texture.height - 1;
                    OLIVEC_PIXEL(oc, x, y) = OLIVEC_PIXEL(texture, texture_x, texture_y);
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
                    int u3 = det - u1 - u2;
                    float z = z1*u1/det + z2*u2/det + z3*(det - u1 - u2)/det;
                    Uv uv = olivec_uv(
                        uv1.u*u1/det + uv2.u*u2/det + uv3.u*u3/det,
                        uv1.v*u1/det + uv2.v*u2/det + uv3.v*u3/det
                    );
                    int texture_x = uv.u/z*texture.width;
                    if (texture_x < 0) texture_x = 0;
                    if ((size_t) texture_x >= texture.width) texture_x = texture.width - 1;
                    int texture_y = uv.v/z*texture.height;
                    if (texture_y < 0) texture_y = 0;
                    if ((size_t) texture_y >= texture.height) texture_y = texture.height - 1;
                    OLIVEC_PIXEL(oc, x, y) = OLIVEC_PIXEL(texture, texture_x, texture_y);
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

// TODO: bilinear interpolation for olivec_sprite_*
OLIVECDEF void olivec_sprite_blend(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite)
{
    if (sprite.width == 0) return;
    if (sprite.height == 0) return;

    Olivec_Normalized_Rect nr = {0};
    if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &nr)) return;

    int xa = nr.ox1; if (w < 0) xa = nr.ox2;
    int ya = nr.oy1; if (h < 0) ya = nr.oy2;
    for (int y = nr.y1; y <= nr.y2; ++y) {
        for (int x = nr.x1; x <= nr.x2; ++x) {
            size_t nx = (x - xa)*((int) sprite.width)/w;
            size_t ny = (y - ya)*((int) sprite.height)/h;
            olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), OLIVEC_PIXEL(sprite, nx, ny));
        }
    }
}

OLIVECDEF void olivec_sprite_copy(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite)
{
    if (sprite.width == 0) return;
    if (sprite.height == 0) return;

    Olivec_Normalized_Rect nr = {0};
    if (!olivec_normalize_rect(x, y, w, h, oc.width, oc.height, &nr)) return;

    int xa = nr.ox1; if (w < 0) xa = nr.ox2;
    int ya = nr.oy1; if (h < 0) ya = nr.oy2;
    for (int y = nr.y1; y <= nr.y2; ++y) {
        for (int x = nr.x1; x <= nr.x2; ++x) {
            size_t nx = (x - xa)*((int) sprite.width)/w;
            size_t ny = (y - ya)*((int) sprite.height)/h;
            OLIVEC_PIXEL(oc, x, y) = OLIVEC_PIXEL(sprite, nx, ny);
        }
    }
}

OLIVECDEF Uv olivec_uv(float u, float v)
{
    Uv uv;
    uv.u = u;
    uv.v = v;
    return uv;
}

#endif // OLIVEC_IMPLEMENTATION

// TODO: Stencil
// TODO: Benchmarking
// TODO: SIMD implementations
// TODO: olivec_ring
// TODO: olivec_ellipse
// TODO: bezier curves
// TODO: fuzzer
