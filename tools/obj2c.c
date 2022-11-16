#include <stdio.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <string.h>

#define SV_IMPLEMENTATION
#include "sv.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define return_defer(value) do { result = (value); goto defer; } while (0)
typedef int Errno;
#define UNUSED(x) (void)(x)

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

void generate_code(FILE *out, Vertices vertices, Faces faces)
{
    fprintf(out, "#ifndef OBJ_H_\n");
    fprintf(out, "#define OBJ_H_\n");
    fprintf(out, "float vertices[%zu][3] = {\n", vertices.count);
    for (size_t i = 0; i < vertices.count; ++i) {
        Vector3 v = vertices.items[i];
        fprintf(out, "    {%f, %f, %f},\n", v.x, v.y, v.z);
    }
    fprintf(out, "};\n");
    fprintf(out, "size_t vertices_count = %zu;\n", vertices.count);

    fprintf(out, "int faces[%zu][3] = {\n", faces.count);
    for (size_t i = 0; i < faces.count; ++i) {
        Face f = faces.items[i];
        fprintf(out, "    {%d, %d, %d},\n", f.a, f.b, f.c);
    }
    fprintf(out, "};\n");
    fprintf(out, "size_t faces_count = %zu;\n", faces.count);
    fprintf(out, "#endif // OBJ_H_\n");
}

// TODO: do not remap any coordinates.
// The Cup 3D demo should just properly scale and place the object in the scene instead.
// Otherwise we end up with distorted model.
Vector3 remap_teapot(Vector3 v, float lx, float hx, float ly, float hy, float lz, float hz)
{
    float scale = 1.0;
    v.z = ((v.z - lz)/(hz - lz)*scale + 1);
    v.x = ((v.x - lx)/(hx - lx)*2 - 1)*scale;
    v.y = ((v.y - ly)/(hy - ly)*2 - 1)*scale;
    return v;
}

int main(int argc, char **argv)
{
    int result = 0;

    if (argc < 3) {
        fprintf(stderr, "Usage: obj2c <input.obj> <output.c>\n");
        fprintf(stderr, "ERROR: no input/output is provide\n");
        return 1;
    }

    const char *obj_file_path = argv[1];
    const char *out_file_path = argv[2];
    char *buffer;
    size_t buffer_size;
    Errno err = read_entire_file(obj_file_path, &buffer, &buffer_size);
    if (err != 0) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", obj_file_path, strerror(errno));
        return_defer(1);
    }

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
    printf("Input:    %s\n", obj_file_path);
    printf("Output:   %s\n", out_file_path);
    printf("Vertices: %zu (x: %f..%f, y: %f..%f, z: %f..%f)\n", vertices.count, lx, hx, ly, hy, lz, hz);
    printf("Faces:    %zu (index: %d..%d)\n", faces.count, lf, hf);

    for (size_t i = 0; i < vertices.count; ++i) {
        vertices.items[i] = remap_teapot(vertices.items[i], lx, hx, ly, hy, lz, hz);
    }

    for (size_t i = 0; i < faces.count; ++i) {
        faces.items[i].a -= 1;
        faces.items[i].b -= 1;
        faces.items[i].c -= 1;
    }

    FILE *out = fopen(out_file_path, "wb");
    if (out == NULL) {
        fprintf(stderr, "ERROR: Could not write file %s: %s\n", out_file_path, strerror(errno));
        return_defer(1);
    }
    generate_code(out, vertices, faces);

defer:
    return result;
}
