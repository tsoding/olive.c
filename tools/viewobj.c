#include <stdio.h>
#include <errno.h>
#include <float.h>
#include <limits.h>

#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define SV_IMPLEMENTATION
#include "sv.h"

static Arena default_arena = {0};
static Arena *context_arena = &default_arena;

static void *context_alloc(size_t size)
{
    assert(context_arena);
    return arena_alloc(context_arena, size);
}

static void *context_realloc(void *oldp, size_t oldsz, size_t newsz)
{
    if (newsz <= oldsz) return oldp;
    return memcpy(context_alloc(newsz), oldp, oldsz);
}

#define FACTOR 120
#define WIDTH (16*FACTOR)
#define HEIGHT (9*FACTOR)

uint32_t pixels[WIDTH*HEIGHT] = {0};
float zbuffer[WIDTH*HEIGHT] = {0};

#define return_defer(value) do { result = (value); goto defer; } while (0)
typedef int Errno;

Errno read_entire_file(const char *file_path, char **buffer, size_t *buffer_size)
{
    Errno result = 0;
    FILE *f = NULL;

    f = fopen(file_path, "rb");
    if (f == NULL) return_defer(errno);

    if (fseek(f, 0, SEEK_END) < 0) return_defer(errno);
    long m = ftell(f);
    if (m < 0) return_defer(errno);
    if (fseek(f, 0, SEEK_SET) < 0) return_defer(errno);

    *buffer_size = m;
    *buffer = context_alloc(*buffer_size);

    fread(*buffer, *buffer_size, 1, f);
    if (ferror(f)) return_defer(errno);

defer:
    if (f) fclose(f);
    return result;
}

typedef struct {
    float x, y;
} Vector2;

Vector2 make_vector2(float x, float y)
{
    Vector2 v2;
    v2.x = x;
    v2.y = y;
    return v2;
}

typedef struct {
    float x, y, z;
} Vector3;

Vector3 make_vector3(float x, float y, float z)
{
    Vector3 v3;
    v3.x = x;
    v3.y = y;
    v3.z = z;
    return v3;
}

Vector2 project_3d_2d(Vector3 v3)
{
    return make_vector2(v3.x / v3.z, v3.y / v3.z);
}

Vector2 project_2d_scr(Vector2 v2)
{
    return make_vector2((v2.x + 1)/2*WIDTH, (1 - (v2.y + 1)/2)*HEIGHT);
}

typedef struct {
    Vector3 *items;
    size_t capacity;
    size_t count;
} Vertices;

typedef struct {
    int a, b, c;
} Face;

Face make_face(int a, int b, int c)
{
    Face f = {
        .a = a,
        .b = b,
        .c = c,
    };
    return f;
}

typedef struct {
    Face *items;
    size_t capacity;
    size_t count;
} Faces;

#define DA_INIT_CAPACITY 8192
#define DA_REALLOC context_realloc
#define da_append(da, item)                                                 \
    do {                                                                    \
        if ((da)->count >= (da)->capacity) {                                \
            size_t new_capacity = (da)->capacity*2;                         \
            if (new_capacity == 0) {                                        \
                new_capacity = DA_INIT_CAPACITY;                            \
            }                                                               \
                                                                            \
            (da)->items = DA_REALLOC((da)->items,                           \
                                     (da)->capacity*sizeof((da)->items[0]), \
                                     new_capacity*sizeof((da)->items[0]));  \
            (da)->capacity = new_capacity;                                  \
        }                                                                   \
                                                                            \
        (da)->items[(da)->count++] = (item);                                \
    } while (0)

#define UNUSED(x) (void)(x)

Vector3 remap_teapot(Vector3 v, float lx, float hx, float ly, float hy, float lz, float hz)
{
    v.z = (v.z - lz)/(hz - lz) + 1;
    v.x = (v.x - lx)/(hx - lx)*2 - 1;
    v.y = (v.y - ly)/(hy - ly)*2 - 1;
    return v;
}

int main(int argc, char **argv)
{
    int result = 0;

    if (argc < 2) {
        fprintf(stderr, "ERROR: no input file is provided\n");
        return_defer(1);
    }

    const char *obj_file_path = argv[1];
    char *buffer;
    size_t buffer_size;
    Errno err = read_entire_file(obj_file_path, &buffer, &buffer_size);
    if (err != 0) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", obj_file_path, strerror(errno));
        return_defer(1);
    }

    // TODO: share code between viewobj and cup3d
    String_View content = sv_from_parts(buffer, buffer_size);
    Vertices vertices = {0};
    Faces faces = {0};
    float lx = FLT_MAX, hx = FLT_MIN;
    float ly = FLT_MAX, hy = FLT_MIN;
    float lz = FLT_MAX, hz = FLT_MIN;
    int lf = INT_MAX, hf = INT_MIN;
    for (size_t line_number = 0; content.count > 0; ++line_number) {
        String_View line = sv_trim_left(sv_chop_by_delim(&content, '\n'));
        if (line.count > 0) {
            String_View kind = sv_chop_by_delim(&line, ' ');
            if (sv_eq(kind, SV("v"))) {
                char *endptr;

                line = sv_trim_left(line);
                // TODO: strtof and strtol imply null terminated strings.
                // But the content is not null terminated. So we rely on the input file having newline at the end.
                // Let's make read_entire_file to append extra \0 or somethign like that
                float x = strtof(line.data, &endptr);
                line.data = endptr;
                if (lx > x) lx = x;
                if (hx < x) hx = x;

                line = sv_trim_left(line);
                float y = strtof(line.data, &endptr);
                line.data = endptr;
                if (ly > y) ly = y;
                if (hy < y) hy = y;

                line = sv_trim_left(line);
                float z = strtof(line.data, &endptr);
                line.data = endptr;
                if (lz > z) lz = z;
                if (hz < z) hz = z;

                da_append(&vertices, make_vector3(x, y, z));
            } else if (sv_eq(kind, SV("f"))) {
                char *endptr;

                line = sv_trim_left(line);
                int a = strtol(line.data, &endptr, 10);
                line.data = endptr;
                if (lf > a) lf = a;
                if (hf < a) hf = a;

                line = sv_trim_left(line);
                int b = strtol(line.data, &endptr, 10);
                line.data = endptr;
                if (lf > b) lf = b;
                if (hf < b) hf = b;

                line = sv_trim_left(line);
                int c = strtol(line.data, &endptr, 10);
                line.data = endptr;
                if (lf > c) lf = c;
                if (hf < c) hf = c;

                da_append(&faces, make_face(a, b, c));
            } else {
                fprintf(stderr, "%s:%zu: unknown kind of entry `"SV_Fmt"`\n", obj_file_path, line_number, SV_Arg(kind));
                return_defer(1);
            }
        }
    }
    printf("Vertices: %zu (x: %f..%f, y: %f..%f, z: %f..%f)\n", vertices.count, lx, hx, ly, hy, lz, hz);
    printf("Faces:    %zu (index: %d..%d)\n", faces.count, lf, hf);

    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
    olivec_fill(oc, 0xFF202020);

    for (size_t i = 0; i < faces.count; ++i) {
        Face f = faces.items[i];
        Vector3 v1 = remap_teapot(vertices.items[f.a - 1], lx, hx, ly, hy, lz, hz);
        Vector3 v2 = remap_teapot(vertices.items[f.b - 1], lx, hx, ly, hy, lz, hz);
        Vector3 v3 = remap_teapot(vertices.items[f.c - 1], lx, hx, ly, hy, lz, hz);
        Vector2 p1 = project_2d_scr(project_3d_2d(v1));
        Vector2 p2 = project_2d_scr(project_3d_2d(v2));
        Vector2 p3 = project_2d_scr(project_3d_2d(v3));

        int lx, hx, ly, hy;
        if (olivec_normalize_triangle(oc.width, oc.height, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, &lx, &hx, &ly, &hy)) {
            for (int y = ly; y <= hy; ++y) {
                for (int x = lx; x <= hx; ++x) {
                    int u1, u2, det;
                    olivec_barycentric(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, x, y, &u1, &u2, &det);
                    int u3 = det - u1 - u2;
                    if (
                        (OLIVEC_SIGN(int, u1) == OLIVEC_SIGN(int, det) || u1 == 0) &&
                        (OLIVEC_SIGN(int, u2) == OLIVEC_SIGN(int, det) || u2 == 0) &&
                        (OLIVEC_SIGN(int, u3) == OLIVEC_SIGN(int, det) || u3 == 0)
                    ) {
                        float z = 1/v1.z*u1/det + 1/v2.z*u2/det + 1/v3.z*(det - u1 - u2)/det;
                        if (z > zbuffer[y*WIDTH + x]) {
                            zbuffer[y*WIDTH + x] = z;
                            OLIVEC_PIXEL(oc, x, y) = mix_colors3(0xFF1818FF, 0xFF18FF18, 0xFFFF1818, u1, u2, det);

                            z = 1.0f/z;
                            if (z >= 1.0) {
                                z -= 1.0;
                                uint32_t v = z*255;
                                if (v > 255) v = 255;
                                olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), (v<<(3*8)));
                            }
                        }
                    }
                }
            }
        }
    }

    // TODO: unhardcode output file path
    const char *file_path = "teapot.png";
    if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t)*WIDTH)) {
        fprintf(stderr, "ERROR: could not write file %s\n", file_path);
        return_defer(1);
    }

defer:
    arena_free(&default_arena);
    return result;
}
