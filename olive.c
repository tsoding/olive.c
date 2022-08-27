#ifndef OLIVE_C_
#define OLIVE_C_

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <png.h>

typedef int Errno;

#define return_defer(value) do { result = (value); goto defer; } while (0)
#define return_defer_png(value) do { result = (value); goto deferpng; } while (0)
#define OLIVEC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define OLIVEC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define OLIVEC_ABS(T, x) (OLIVEC_SIGN(T, x)*(x))

void olivec_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{
    for (size_t i = 0; i < width*height; ++i) {
        pixels[i] = color;
    }
}

Errno olivec_save_to_png_file(uint32_t *pixels, size_t width, size_t height, const char *file_path)
{
    int result = 0;
    png_byte **row_pointers = NULL;
    png_structp png = NULL;
    png_infop info = NULL;
    FILE *f = NULL;
    int pixel_size = 3;
    int depth = 8;
    size_t x, y;

    f = fopen(file_path, "wb");
    if (f == NULL) return_defer_png(errno);

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL) return_defer_png(errno);

    info = png_create_info_struct(png);
    if (info == NULL) return_defer_png(errno);

    if (setjmp(png_jmpbuf(png)))
    {
       return_defer_png(errno);
    }

    // Output is 8 bit depth, RGB format.
    png_set_IHDR(
        png,
        info,
        width, height,
        depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
        );

    row_pointers = png_malloc(png, height * sizeof(png_byte*));

    uint32_t* p = (uint32_t*)pixels;

    for (y = 0; y < height; y++)
    {
        png_byte *row = png_malloc (png, sizeof(uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < width; x++)
        {
            int ix = y * width + x;
            uint32_t pixel = p[ix];

            *row++ = (pixel & 0x0000FF) >> 0;
            *row++ = (pixel & 0x00FF00) >> 8;
            *row++ = (pixel & 0xFF0000) >> 16;
        }
    }

    png_init_io(png, f);
    png_set_rows(png, info, row_pointers);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png, NULL);

    for (y = 0; y < height; y++)
    {
        png_free(png, row_pointers[y]);
    }

    png_destroy_write_struct(&png, &info);

deferpng:
    if (f) fclose(f);
    return result;
}

Errno olivec_save_to_ppm_file(uint32_t *pixels, size_t width, size_t height, const char *file_path)
{
    int result = 0;
    FILE *f = NULL;

    {
        f = fopen(file_path, "wb");
        if (f == NULL) return_defer(errno);

        fprintf(f, "P6\n%zu %zu 255\n", width, height);
        if (ferror(f)) return_defer(errno);

        for (size_t i = 0; i < width*height; ++i) {
            uint32_t pixel = pixels[i];
            uint8_t bytes[3] = {
                (pixel>>(8*0))&0xFF,
                (pixel>>(8*1))&0xFF,
                (pixel>>(8*2))&0xFF,
            };
            fwrite(bytes, sizeof(bytes), 1, f);
            if (ferror(f)) return_defer(errno);
        }
    }

defer:
    if (f) fclose(f);
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
    int x2 = x1 + OLIVEC_SIGN(int, w)*(OLIVEC_ABS(int, w) - 1);
    if (x1 > x2) OLIVEC_SWAP(int, x1, x2);
    int y2 = y1 + OLIVEC_SIGN(int, h)*(OLIVEC_ABS(int, h) - 1);
    if (y1 > y2) OLIVEC_SWAP(int, y1, y2);

    for (int y = y1; y <= y2; ++y) {
        // TODO: move boundary checks out of the loops in olivec_fill_rect
        if (0 <= y && y < (int) pixels_height) {
            for (int x = x1; x <= x2; ++x) {
                if (0 <= x && x < (int) pixels_width) {
                    pixels[y*pixels_width + x] = color;
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
    if (r == 0) return;

    int x1 = cx - r;
    int x2 = cx + r;
    if (x1 > x2) OLIVEC_SWAP(int, x1, x2);

    int y1 = cy - r;
    int y2 = cy + r;
    if (y1 > y2) OLIVEC_SWAP(int, y1, y2);

    for (int y = y1; y <= y2; ++y) {
        // TODO: move boundary checks out of the loops in olivec_fill_circle
        if (0 <= y && y < (int) pixels_height) {
            for (int x = x1; x <= x2; ++x) {
                if (0 <= x && x < (int) pixels_width) {
                    int dx = x - cx;
                    int dy = y - cy;
                    if (dx*dx + dy*dy <= r*r) {
                        pixels[y*pixels_width + x] = color;
                    }
                }
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
            if (0 <= x && x < (int) pixels_width) {
                int sy1 = dy*x/dx + c;
                int sy2 = dy*(x + 1)/dx + c;
                if (sy1 > sy2) OLIVEC_SWAP(int, sy1, sy2);
                for (int y = sy1; y <= sy2; ++y) {
                    if (0 <= y && y < (int) pixels_height) {
                        pixels[y*pixels_width + x] = color;
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
                    pixels[y*pixels_width + x] = color;
                }
            }
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
