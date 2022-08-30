#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#include "olive.c"

#define return_defer(value) do { result = (value); goto defer; } while (0)
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(message) \
    do { \
        fprintf(stderr, "%s:%d: UNIMPLEMENTED: %s\n", __FILE__, __LINE__, message); \
        exit(1); \
    } while (0)
#define UNREACHABLE(message) \
    do { \
        fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); \
        exit(1); \
    } while (0)

#define WIDTH 128
#define HEIGHT 128

#define BACKGROUND_COLOR 0xFF202020
#define RED_COLOR 0xFF2020AA
#define GREEN_COLOR 0xFF20AA20
#define BLUE_COLOR 0xFFAA2020
#define ERROR_COLOR 0xFFFF00FF

#define TEST_DIR_PATH "./test"

uint32_t pixels[WIDTH*HEIGHT];

bool record_test_case(const char *file_path)
{
    if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t)*WIDTH)) {
        fprintf(stderr, "ERROR: could not write file %s: %s\n", file_path, strerror(errno));
        return(false);
    }
    printf("Generated %s\n", file_path);
    return(true);
}

bool replay_test_case(const char *program_path, const char *file_path, const char *failure_file_path)
{
    bool result = true;
    uint32_t *expected_pixels = NULL;

    {
        int expected_width, expected_height;
        expected_pixels = (uint32_t*) stbi_load(file_path, &expected_width, &expected_height, NULL, 4);
        if (expected_pixels == NULL) {
            fprintf(stderr, "%s: TEST FAILURE: could not read the file: %s\n", file_path, strerror(errno));
            if (errno == ENOENT) {
                fprintf(stderr, "%s: HINT: Consider running `%s record` to create it\n", file_path, program_path);
            }
            return_defer(false);
        }

        // TODO: it would be cool if "unexpected image size" error would generate the image diff as well
        if (expected_width != WIDTH || expected_height != HEIGHT) {
            fprintf(stderr, "%s: TEST FAILURE: unexpected image size. Expected %dx%d, but got %dx%d\n",
                    file_path, expected_width, expected_height, WIDTH, HEIGHT);
            return_defer(false);
        }

        bool failed = false;
        for (size_t y = 0; y < HEIGHT; ++y) {
            for (size_t x = 0; x < WIDTH; ++x) {
                uint32_t expected_pixel = expected_pixels[y*WIDTH + x];
                uint32_t actual_pixel = pixels[y*WIDTH + x];
                if (expected_pixel != actual_pixel) {
                    pixels[y*WIDTH + x] = ERROR_COLOR;
                    failed = true;
                }
            }
        }

        if (failed) {
            fprintf(stderr, "%s: TEST FAILURE: unexpected pixels in generated image\n", file_path);
            if (!stbi_write_png(failure_file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t)*WIDTH)) {
                fprintf(stderr, "ERROR: could not generate image diff %s: %s\n", failure_file_path, strerror(errno));
            } else {
                printf("See image diff %s for more info\n", failure_file_path);
            }
            return_defer(false);
        }

        printf("%s OK\n", file_path);
    }

defer:
    if (expected_pixels) stbi_image_free(expected_pixels);
    return result;
}

typedef struct {
    void (*run)(void);
    const char *file_path;
    const char *failure_file_path;
} Test_Case;

#define DEFINE_TEST_CASE(name) \
    { \
        .run = name, \
        .file_path = TEST_DIR_PATH "/" #name ".png", \
        .failure_file_path = TEST_DIR_PATH "/" #name "_failure.png" \
    }

void test_fill_rect(void)
{
    olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);
    olivec_fill_rect(pixels, WIDTH, HEIGHT, WIDTH/2 - WIDTH/8, HEIGHT/2 - HEIGHT/8, WIDTH/4, HEIGHT/4, RED_COLOR);
    olivec_fill_rect(pixels, WIDTH, HEIGHT, WIDTH - 1, HEIGHT - 1, -WIDTH/2, -HEIGHT/2, GREEN_COLOR);
    olivec_fill_rect(pixels, WIDTH, HEIGHT, -WIDTH/4, -HEIGHT/4, WIDTH/2, HEIGHT/2, BLUE_COLOR);
}

void test_fill_circle(void)
{
    olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, 0, 0, WIDTH/2, RED_COLOR);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, WIDTH/4, BLUE_COLOR);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, WIDTH*3/4, HEIGHT*3/4, -WIDTH/4, GREEN_COLOR);
}

void test_draw_line(void)
{
    olivec_fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);
    olivec_draw_line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, RED_COLOR);
    olivec_draw_line(pixels, WIDTH, HEIGHT, WIDTH, 0, 0, HEIGHT, RED_COLOR);
}

Test_Case test_cases[] = {
    DEFINE_TEST_CASE(test_fill_rect),
    DEFINE_TEST_CASE(test_fill_circle),
    DEFINE_TEST_CASE(test_draw_line),
};
#define TEST_CASES_COUNT (sizeof(test_cases)/sizeof(test_cases[0]))

int main(int argc, char **argv)
{
    assert(argc >= 1);
    const char *program_path = argv[0];
    bool record = argc >= 2 && strcmp(argv[1], "record") == 0;

    for (size_t i = 0; i < TEST_CASES_COUNT; ++i) {
        test_cases[i].run();
        if (record) {
            if (!record_test_case(test_cases[i].file_path)) return 1;
        } else {
            if (!replay_test_case(program_path, test_cases[i].file_path, test_cases[i].failure_file_path)) return 1;
        }
    }
    return 0;
}
