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

bool replay_test_case(const char *file_path, const char *failure_file_path)
{
    bool result = true;
    uint32_t *expected_pixels = NULL;

    {
        int expected_width, expected_height;
        expected_pixels = (uint32_t*) stbi_load(file_path, &expected_width, &expected_height, NULL, 4);
        if (expected_pixels == NULL) {
            fprintf(stderr, "ERROR: could not read file %s: %s\n", file_path, strerror(errno));
            return_defer(false);
        }

        if (expected_width != WIDTH || expected_height != HEIGHT) {
            fprintf(stderr, "%s: FAILURE: unexpected image size. Expected %dx%d, but got %dx%d\n",
                    file_path, expected_width, expected_height, WIDTH, HEIGHT);
            return_defer(false);
        }

        bool failed = false;
        for (size_t y = 0; y < HEIGHT; ++y) {
            for (size_t x = 0; x < WIDTH; ++x) {
                uint32_t expected_pixel = expected_pixels[y*WIDTH + x];
                uint32_t actual_pixel = pixels[y*WIDTH + x];
                if (expected_pixel != actual_pixel) {
                    pixels[y*WIDTH + x] = 0xFF0000FF;
                    failed = true;
                }
            }
        }

        if (failed) {
            fprintf(stderr, "%s: FAILURE: unexpected pixels in generated image\n", file_path);
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
    olivec_fill(pixels, WIDTH, HEIGHT, 0xFF202020);
    olivec_fill_rect(pixels, WIDTH, HEIGHT, WIDTH/2 - WIDTH/8, HEIGHT/2 - HEIGHT/8, WIDTH/4, HEIGHT/4, 0xFF2020AA);
    olivec_fill_rect(pixels, WIDTH, HEIGHT, WIDTH - 1, HEIGHT - 1, -WIDTH/2, -HEIGHT/2, 0xFF20AA20);
    olivec_fill_rect(pixels, WIDTH, HEIGHT, -WIDTH/4, -HEIGHT/4, WIDTH/2, HEIGHT/2, 0xFFAA2020);
}

void test_fill_circle(void)
{
    olivec_fill(pixels, WIDTH, HEIGHT, 0xFF202020);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, 0, 0, WIDTH/2, 0xFF2020AA);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, WIDTH/4, 0xFFAA2020);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, WIDTH*3/4, HEIGHT*3/4, -WIDTH/4, 0xFF20AA20);
}

Test_Case test_cases[] = {
    DEFINE_TEST_CASE(test_fill_rect),
    DEFINE_TEST_CASE(test_fill_circle),
};
#define TEST_CASES_COUNT (sizeof(test_cases)/sizeof(test_cases[0]))

int main(int argc, char **argv)
{
    bool record = argc >= 2 && strcmp(argv[1], "record") == 0;

    for (size_t i = 0; i < TEST_CASES_COUNT; ++i) {
        test_cases[i].run();
        if (record) {
            if (!record_test_case(test_cases[i].file_path)) return 1;
        } else {
            if (!replay_test_case(test_cases[i].file_path, test_cases[i].failure_file_path)) return 1;
        }
    }
    return 0;
}
