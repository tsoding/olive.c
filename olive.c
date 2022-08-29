#ifndef OLIVE_C_
#define OLIVE_C_

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

typedef int Errno;

#define return_defer(value) \
    do                      \
    {                       \
        result = (value);   \
        goto defer;         \
    } while (0)
#define OLIVEC_SWAP(T, a, b) \
    do                       \
    {                        \
        T t = a;             \
        a = b;               \
        b = t;               \
    } while (0)
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x) * (x))

void olivec_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{
    for (size_t i = 0; i < width * height; ++i)
    {
        pixels[i] = color;
    }
}

Errno olivec_save_to_ppm_file(uint32_t *pixels, size_t width, size_t height, const char *file_path)
{
    int result = 0;
    FILE *f = NULL;

    {
        f = fopen(file_path, "wb");
        if (f == NULL)
            return_defer(errno);

        fprintf(f, "P6\n%zu %zu 255\n", width, height);
        if (ferror(f))
            return_defer(errno);

        for (size_t i = 0; i < width * height; ++i)
        {
            uint32_t pixel = pixels[i];
            uint8_t bytes[3] = {
                (pixel >> (8 * 0)) & 0xFF,
                (pixel >> (8 * 1)) & 0xFF,
                (pixel >> (8 * 2)) & 0xFF,
            };
            fwrite(bytes, sizeof(bytes), 1, f);
            if (ferror(f))
                return_defer(errno);
        }
    }

defer:
    if (f)
        fclose(f);
    return result;
}

// TODO: implement automatic test for olivec_fill_rect
// Check for
//  - negative width and height of the rect
//  - out-of-bound cases
void olivec_fill_rect(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x1, int y1, int w, int h,
                      uint32_t color)
{
    int x2 = x1 + OLIVEC_SIGN(int, w) * (OLIVEC_ABS(int, w) - 1);
    if (x1 > x2)
        OLIVEC_SWAP(int, x1, x2);
    int y2 = y1 + OLIVEC_SIGN(int, h) * (OLIVEC_ABS(int, h) - 1);
    if (y1 > y2)
        OLIVEC_SWAP(int, y1, y2);

    for (int y = y1; y <= y2; ++y)
    {
        // TODO: move boundary checks out of the loops in olivec_fill_rect
        if (0 <= y && y < (int)pixels_height)
        {
            for (int x = x1; x <= x2; ++x)
            {
                if (0 <= x && x < (int)pixels_width)
                {
                    pixels[y * pixels_width + x] = color;
                }
            }
        }
    }
}

// TODO: implement automatic test for olivec_fill_circle
// Check for
//  - negative radius of the circle
//  - out-of-bound cases
void olivec_fill_circle(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                        int cx, int cy, int r,
                        uint32_t color)
{
    if (r == 0)
        return;

    int x1 = cx - r;
    int x2 = cx + r;
    if (x1 > x2)
        OLIVEC_SWAP(int, x1, x2);

    int y1 = cy - r;
    int y2 = cy + r;
    if (y1 > y2)
        OLIVEC_SWAP(int, y1, y2);

    for (int y = y1; y <= y2; ++y)
    {
        // TODO: move boundary checks out of the loops in olivec_fill_circle
        if (0 <= y && y < (int)pixels_height)
        {
            for (int x = x1; x <= x2; ++x)
            {
                if (0 <= x && x < (int)pixels_width)
                {
                    int dx = x - cx;
                    int dy = y - cy;
                    if (dx * dx + dy * dy <= r * r)
                    {
                        pixels[y * pixels_width + x] = color;
                    }
                }
            }
        }
    }
}

void olivec_draw_pixel(uint32_t *pixels, size_t pixels_width, size_t pixels_height, int x, int y, uint32_t color)
{
    if (x < 0 || x >= (int)pixels_width || y < 0 || y >= (int)pixels_height)
        return;

    pixels[y * pixels_width + x] = color;
}

// TODO: lines with different thicness
void olivec_draw_line(uint32_t *pixels, size_t pixels_width, size_t pixels_height,
                      int x1, int y1, int x2, int y2,
                      uint32_t color)
{
    bool steep = abs(y2 - y1) > abs(x2 - x1);

    if (steep)
    {
        OLIVEC_SWAP(int, x1, y1);
        OLIVEC_SWAP(int, x2, y2);
    }

    if (x1 > x2)
    {
        OLIVEC_SWAP(int, x1, x2);
        OLIVEC_SWAP(int, y1, y2);
    }

    int deltay;
    if (y1 > y2)
        deltay = -1;
    else
        deltay = 1;

    int dx = x2 - x1;
    int dy = abs(y2 - y1);
    int y = y1;
    int error = 0;

    for (int x = x1; x < x2; x++)
    {
        if (x < 0 || x >= (int)pixels_width)
            continue;

        if (steep)
            olivec_draw_pixel(pixels, pixels_width, pixels_height, y, x, color);
        else
            olivec_draw_pixel(pixels, pixels_width, pixels_height, x, y, color);

        error += dy;
        if (2 * error >= dx)
        {
            y = y + deltay;
            error -= dx;
        }
    }
}

// TODO: supersampling for circles and lines
// TODO: olivec_fill_triangle
// TODO: olivec_draw_circle
// TODO: olivec_(draw|fill)_ellipse
// TODO: WebAssembly Demo
// TODO: Alpha blending

#endif // OLIVE_C_
