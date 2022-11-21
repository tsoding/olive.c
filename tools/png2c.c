#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#define return_defer(value) do { result = (value); goto defer; } while (0)

const char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);
    const char *result = *argv[0];
    *argc -= 1;
    *argv += 1;
    return result;
}

void usage(FILE *out, const char *program_name)
{
    fprintf(out, "Usage: %s [OPTIONS] <input/file/path.png>\n", program_name);
    fprintf(out, "Options:\n");
    fprintf(out, "    -o <output/file/path.h>\n");
    fprintf(out, "    -n <name>\n");
}

void generate_c_code_from_pixels(FILE *out, uint32_t *data, int x, int y, const char *name)
{
    size_t name_len = strlen(name);
    char *capital_name = malloc(name_len + 1);
    assert(capital_name != NULL && "Buy more RAM, I guess");
    for (size_t i = 0; i < name_len; ++i) {
        capital_name[i] = toupper(name[i]);
    }
    capital_name[name_len] = '\0';

    fprintf(out, "#ifndef %s_H_\n", capital_name);
    fprintf(out, "#define %s_H_\n", capital_name);
    fprintf(out, "size_t %s_width = %d;\n", name, x);
    fprintf(out, "size_t %s_height = %d;\n", name, y);
    fprintf(out, "uint32_t %s_pixels[] = {\n", name);
    size_t length = (size_t)(x * y);
    size_t width = 7;
    for (size_t i = 0; i < (length + width - 1)/width; ++i) {
        fprintf(out, "   ");
        for (size_t j = 0; j < width && i*width + j < length; ++j) {
            fprintf(out, "0x%08X,", data[i*width + j]);
        }
        fprintf(out, "\n");
    }
    fprintf(out, "};\n");
    fprintf(out, "#endif // %s_H_\n", capital_name);

//defer:
    free(capital_name);
}

bool generate_c_file_from_png(const char *input_file_path, const char *output_file_path, const char *name)
{
    bool result = true;

    FILE *out = NULL;
    uint32_t *data = NULL;

    {
        int x, y;
        data = (uint32_t *)stbi_load(input_file_path, &x, &y, NULL, 4);

        if (data == NULL) {
            fprintf(stderr, "ERROR: Could not load file `%s`: %s\n", input_file_path, stbi_failure_reason());
            return_defer(false);
        }

        if (output_file_path) {
            out = fopen(output_file_path, "wb");
            if (out == NULL) {
                fprintf(stderr, "ERROR: could not write to file `%s`: %s\n", output_file_path, strerror(errno));
                return_defer(false);
            }
            generate_c_code_from_pixels(out, data, x, y, name);
        } else {
            generate_c_code_from_pixels(stdout, data, x, y, name);
        }
    }

defer:
    if (out) fclose(out);
    if (data) stbi_image_free(data);
    return result;
}

int main(int argc, char *argv[])
{
    assert(argc > 0);
    const char *program_name = shift(&argc, &argv);
    const char *output_file_path = NULL;
    const char *input_file_path = NULL;
    const char *name = NULL;

    while (argc > 0) {
        const char *flag = shift(&argc, &argv);
        if (strcmp(flag, "-o") == 0) {
            if (argc <= 0) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: no value is provided for flag %s\n", flag);
                return 1;
            }

            if (output_file_path != NULL) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: %s was already provided\n", flag);
                return 1;
            }

            output_file_path = shift(&argc, &argv);
        } else if (strcmp(flag, "-n") == 0) {
            if (argc <= 0) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: no value is provided for flag %s\n", flag);
                return 1;
            }

            if (name != NULL) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: %s was already provided\n", flag);
                return 1;
            }

            name = shift(&argc, &argv);
        } else {
            if (input_file_path != NULL) {
                usage(stderr, program_name);
                fprintf(stderr, "ERROR: input file path was already provided\n");
                return 1;
            }
            input_file_path = flag;
        }
    }

    if (input_file_path == NULL) {
        usage(stderr, program_name);
        fprintf(stderr, "ERROR: expected input file path\n");
        return(1);
    }

    if (name == NULL) {
        name = "png";
    } else {
        size_t n = strlen(name);
        if (n == 0) {
            fprintf(stderr, "ERROR: name cannot be empty\n");
            return 1;
        }

        if (isdigit(name[0])) {
            fprintf(stderr, "ERROR: name cannot start from a digit\n");
            return 1;
        }

        for (size_t i = 0; i < n; ++i) {
            if (!isalnum(name[i]) && name[i] != '_') {
                fprintf(stderr, "ERROR: name can only contains alphanumeric characters and underscores\n");
                return 1;
            }
        }
    }

    if (!generate_c_file_from_png(input_file_path, output_file_path, name)) return 1;

    return 0;
}
