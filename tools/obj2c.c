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

const char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    const char *result = *argv[0];
    *argc -= 1;
    *argv += 1;
    return result;
}

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
    *buffer = context_alloc(*buffer_size + 1);
    (*buffer)[*buffer_size] = '\0';

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
    fprintf(out, "#define vertices_count %zu\n", vertices.count);
    fprintf(out, "static const float vertices[][3] = {\n");
    for (size_t i = 0; i < vertices.count; ++i) {
        Vector3 v = vertices.items[i];
        fprintf(out, "    {%f, %f, %f},\n", v.x, v.y, v.z);
    }
    fprintf(out, "};\n");

    fprintf(out, "static const int faces[%zu][3] = {\n", faces.count);
    for (size_t i = 0; i < faces.count; ++i) {
        Face f = faces.items[i];
        fprintf(out, "    {%d, %d, %d},\n", f.a, f.b, f.c);
    }
    fprintf(out, "};\n");
    fprintf(out, "#define faces_count %zu\n", faces.count);
    fprintf(out, "#endif // OBJ_H_\n");
}

Vector3 remap_object(Vector3 v, float scale, float lx, float hx, float ly, float hy, float lz, float hz)
{
    float cx = lx + (hx - lx)/2;
    float cy = ly + (hy - ly)/2;
    float cz = lz + (hz - lz)/2;
    v.z = (v.z - cz)*scale;
    v.x = (v.x - cx)*scale;
    v.y = (v.y - cy)*scale;
    return v;
}

void usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s [OPTIONS] <INPUT.obj>\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -o    output file path\n");
    fprintf(stderr, "    -s    scale the model\n");
}

int main(int argc, char **argv)
{
    int result = 0;

    assert(argc > 0);
    const char *program_name = shift(&argc, &argv);
    const char *output_file_path = NULL;
    const char *input_file_path = NULL;
    float scale = 0.75;

    // TODO: consider using https://github.com/tsoding/flag.h in here
    while (argc > 0) {
        const char *flag = shift(&argc, &argv);
        if (strcmp(flag, "-o") == 0) {
            if (argc <= 0) {
                usage(program_name);
                fprintf(stderr, "ERROR: no value is provided for flag %s\n", flag);
                return_defer(1);
            }

            if (output_file_path != NULL) {
                usage(program_name);
                fprintf(stderr, "ERROR: %s was already provided\n", flag);
                return_defer(1);
            }

            output_file_path = shift(&argc, &argv);
        } else if (strcmp(flag, "-s") == 0) {
            if (argc <= 0) {
                usage(program_name);
                fprintf(stderr, "ERROR: no value is provided for flag %s\n", flag);
                return_defer(1);
            }

            const char *value = shift(&argc, &argv);
            scale = strtof(value, NULL);
        } else {
            if (input_file_path != NULL) {
                usage(program_name);
                fprintf(stderr, "ERROR: input file path was already provided\n");
                return_defer(1);
            }
            input_file_path = flag;
        }
    }

    if (input_file_path == NULL) {
        usage(program_name);
        fprintf(stderr, "ERROR: no input file path is provided\n");
        return_defer(1);
    }

    if (output_file_path == NULL) {
        usage(program_name);
        fprintf(stderr, "ERROR: no output file path is provided\n");
        return_defer(1);
    }

    char *buffer;
    size_t buffer_size;
    Errno err = read_entire_file(input_file_path, &buffer, &buffer_size);
    if (err != 0) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", input_file_path, strerror(errno));
        return_defer(1);
    }

    String_View content = sv_from_parts(buffer, buffer_size);
    Vertices vertices = {0};
    Faces faces = {0};
    size_t normals_counts = 0;
    size_t texture_coords_count = 0;
    float lx = FLT_MAX, hx = FLT_MIN;
    float ly = FLT_MAX, hy = FLT_MIN;
    float lz = FLT_MAX, hz = FLT_MIN;
    int lf = INT_MAX, hf = INT_MIN;
    bool one_object_encountered = false;
    size_t one_object_line_number = 0;
    for (size_t line_number = 1; content.count > 0; ++line_number) {
        String_View line = sv_trim_left(sv_chop_by_delim(&content, '\n'));
        if (line.count > 0 && *line.data != '#') {
            String_View kind = sv_chop_by_delim(&line, ' ');
            if (sv_eq(kind, SV("v"))) {
                char *endptr;

                line = sv_trim_left(line);
                float x = strtof(line.data, &endptr);
                if (lx > x) lx = x;
                if (hx < x) hx = x;
                sv_chop_left(&line, endptr - line.data);

                line = sv_trim_left(line);
                float y = strtof(line.data, &endptr);
                if (ly > y) ly = y;
                if (hy < y) hy = y;
                sv_chop_left(&line, endptr - line.data);

                line = sv_trim_left(line);
                float z = strtof(line.data, &endptr);
                if (lz > z) lz = z;
                if (hz < z) hz = z;
                sv_chop_left(&line, endptr - line.data);

                da_append(&vertices, make_vector3(x, y, z));
            } else if (sv_eq(kind, SV("f"))) {
                char *endptr;

                // TODO: Parse format f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3

                line = sv_trim_left(line);
                int a = strtol(line.data, &endptr, 10);
                if (lf > a) lf = a;
                if (hf < a) hf = a;
                sv_chop_left(&line, endptr - line.data);
                while (line.count > 0 && !isspace(*line.data)) sv_chop_left(&line, 1);

                line = sv_trim_left(line);
                int b = strtol(line.data, &endptr, 10);
                if (lf > b) lf = b;
                if (hf < b) hf = b;
                sv_chop_left(&line, endptr - line.data);
                while (line.count > 0 && !isspace(*line.data)) sv_chop_left(&line, 1);

                line = sv_trim_left(line);
                int c = strtol(line.data, &endptr, 10);
                if (lf > c) lf = c;
                if (hf < c) hf = c;
                sv_chop_left(&line, endptr - line.data);
                while (line.count > 0 && !isspace(*line.data)) sv_chop_left(&line, 1);

                da_append(&faces, make_face(a, b, c));
            } else if (sv_eq(kind, SV("mtllib"))) {
                fprintf(stderr, "%s:%zu: WARNING: mtllib is not supported yet. Ignoring it...\n", input_file_path, line_number);
            } else if (sv_eq(kind, SV("usemtl"))) {
                fprintf(stderr, "%s:%zu: WARNING: usemtl is not supported yet. Ignoring it...\n", input_file_path, line_number);
            } else if (sv_eq(kind, SV("o"))) {
                if (one_object_encountered) {
                    fprintf(stderr, "%s:%zu: ERROR: %s supports only one object as of right now.\n", input_file_path, line_number, program_name);
                    fprintf(stderr, "%s:%zu: NOTE: we already processing this object\n", input_file_path, one_object_line_number);
                    return_defer(1);
                }
                line = sv_trim_left(line);
                String_View name = line;
                fprintf(stderr, "%s:%zu: INFO: processing object `"SV_Fmt"`\n", input_file_path, line_number, SV_Arg(name));
                one_object_encountered = true;
                one_object_line_number = line_number;
            } else if (sv_eq(kind, SV("s"))) {
                fprintf(stderr, "%s:%zu: WARNING: smooth groups are not supported right now. Ignoring them...\n", input_file_path, line_number);
            } else if (sv_eq(kind, SV("vn"))) {
                normals_counts += 1;
            } else if (sv_eq(kind, SV("vt"))) {
                texture_coords_count += 1;
            } else {
                fprintf(stderr, "%s:%zu: ERROR: unknown kind of entry `"SV_Fmt"`\n", input_file_path, line_number, SV_Arg(kind));
                return_defer(1);
            }
        }
    }
    printf("Input:               %s\n", input_file_path);
    printf("Output:              %s\n", output_file_path);
    printf("Vertices:            %zu (x: %f..%f, y: %f..%f, z: %f..%f)\n", vertices.count, lx, hx, ly, hy, lz, hz);
    printf("Normals:             %zu\n", normals_counts);
    printf("Texture Coordinates: %zu\n", texture_coords_count);
    printf("Faces:               %zu (index: %d..%d)\n", faces.count, lf, hf);

    for (size_t i = 0; i < vertices.count; ++i) {
        vertices.items[i] = remap_object(vertices.items[i], scale, lx, hx, ly, hy, lz, hz);
    }

    for (size_t i = 0; i < faces.count; ++i) {
        faces.items[i].a -= 1;
        faces.items[i].b -= 1;
        faces.items[i].c -= 1;
    }

    FILE *out = fopen(output_file_path, "wb");
    if (out == NULL) {
        fprintf(stderr, "ERROR: Could not write file %s: %s\n", output_file_path, strerror(errno));
        return_defer(1);
    }
    generate_code(out, vertices, faces);

defer:
    return result;
}
