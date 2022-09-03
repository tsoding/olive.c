#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#define OLIVEC_IMPLEMENTATION
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

char hexchar(uint8_t x)
{
    if (x < 10) return x + '0';
    if (x < 16) return x - 10 + 'A';
    UNREACHABLE("hexchar");
}

const char *display_hexcolor(uint32_t c)
{
    static char buffer[1 + 8 + 1];
    buffer[0] = '#';
    buffer[1] = hexchar((c>>(1*4))&0xF);
    buffer[2] = hexchar((c>>(0*4))&0xF);
    buffer[3] = hexchar((c>>(3*4))&0xF);
    buffer[4] = hexchar((c>>(2*4))&0xF);
    buffer[5] = hexchar((c>>(5*4))&0xF);
    buffer[6] = hexchar((c>>(4*4))&0xF);
    buffer[7] = hexchar((c>>(7*4))&0xF);
    buffer[8] = hexchar((c>>(6*4))&0xF);
    buffer[9] = '\0';
    return buffer;
}

static uint32_t actual_pixels[WIDTH*HEIGHT];
static uint32_t diff_pixels[WIDTH*HEIGHT];

bool record_test_case(const char *expected_file_path)
{
    if (!stbi_write_png(expected_file_path, WIDTH, HEIGHT, 4, actual_pixels, sizeof(uint32_t)*WIDTH)) {
        fprintf(stderr, "ERROR: could not write file %s: %s\n", expected_file_path, strerror(errno));
        return(false);
    }
    printf("Generated %s\n", expected_file_path);
    return(true);
}

typedef enum {
    REPLAY_PASSED,
    REPLAY_FAILED,
    REPLAY_ERRORED,
} Replay_Result;

Replay_Result replay_test_case(const char *program_path, const char *expected_file_path, const char *actual_file_path, const char *diff_file_path)
{
    Replay_Result result = REPLAY_PASSED;
    uint32_t *expected_pixels = NULL;

    {
        int expected_width, expected_height;
        expected_pixels = (uint32_t*) stbi_load(expected_file_path, &expected_width, &expected_height, NULL, 4);
        if (expected_pixels == NULL) {
            fprintf(stderr, "%s: ERROR: could not read the file: %s\n", expected_file_path, strerror(errno));
            if (errno == ENOENT) {
                fprintf(stderr, "%s: HINT: Consider running `$ %s record` to create it\n", expected_file_path, program_path);
            }
            return_defer(REPLAY_ERRORED);
        }

        // TODO: it would be cool if "unexpected image size" error would generate the image diff as well
        // The size of the image diff should be max(expected_width, actual_width) by max(expected_height, actual_height) with the paddings on the right and bottom edges filled with ERROR_COLOR
        if (expected_width != WIDTH || expected_height != HEIGHT) {
            fprintf(stderr, "%s: TEST FAILURE: unexpected image size. Expected %dx%d, but got %dx%d\n",
                    expected_file_path, expected_width, expected_height, WIDTH, HEIGHT);
            return_defer(REPLAY_FAILED);
        }

        bool failed = false;
        for (size_t y = 0; y < HEIGHT; ++y) {
            for (size_t x = 0; x < WIDTH; ++x) {
                uint32_t expected_pixel = expected_pixels[y*WIDTH + x];
                uint32_t actual_pixel = actual_pixels[y*WIDTH + x];
                if (expected_pixel != actual_pixel) {
                    diff_pixels[y*WIDTH + x] = ERROR_COLOR;
                    failed = true;
                } else {
                    diff_pixels[y*WIDTH + x] = expected_pixel;
                }
            }
        }

        if (failed) {
            fprintf(stderr, "%s: TEST FAILURE: unexpected pixels in generated image\n", expected_file_path);

            if (!stbi_write_png(actual_file_path, WIDTH, HEIGHT, 4, actual_pixels, sizeof(uint32_t)*WIDTH)) {
                fprintf(stderr, "ERROR: could not generate image with actual pixels %s: %s\n", actual_file_path, strerror(errno));
                return_defer(REPLAY_ERRORED);
            }

            if (!stbi_write_png(diff_file_path, WIDTH, HEIGHT, 4, diff_pixels, sizeof(uint32_t)*WIDTH)) {
                fprintf(stderr, "ERROR: could not generate diff image %s: %s\n", diff_file_path, strerror(errno));
                return_defer(REPLAY_ERRORED);
            }

            fprintf(stderr, "%s: HINT: See actual image %s\n", expected_file_path, actual_file_path);
            fprintf(stderr, "%s: HINT: See diff image %s\n", expected_file_path, diff_file_path);
            fprintf(stderr, "%s: HINT: If this behaviour is intentional confirm that by updating the image with `$ %s record`\n", expected_file_path, program_path);
            return_defer(REPLAY_FAILED);
        }

        printf("%s OK\n", expected_file_path);
    }

defer:
    if (expected_pixels) stbi_image_free(expected_pixels);
    return result;
}

typedef struct {
    void (*generate_actual_pixels)(void);
    const char *expected_file_path;
    const char *actual_file_path;
    const char *diff_file_path;
} Test_Case;

#define DEFINE_TEST_CASE(name) \
    { \
        .generate_actual_pixels = name, \
        .expected_file_path = TEST_DIR_PATH "/" #name "_expected.png", \
        .actual_file_path = TEST_DIR_PATH "/" #name "_actual.png", \
        .diff_file_path = TEST_DIR_PATH "/" #name "_diff.png", \
    }

void test_fill_rect(void)
{
    Olivec_Canvas oc = olivec_canvas(actual_pixels, WIDTH, HEIGHT);
    olivec_fill(oc, BACKGROUND_COLOR);
    olivec_rect(oc, WIDTH/2 - WIDTH/8, HEIGHT/2 - HEIGHT/8, WIDTH/4, HEIGHT/4, RED_COLOR);
    olivec_rect(oc, WIDTH - 1, HEIGHT - 1, -WIDTH/2, -HEIGHT/2, GREEN_COLOR);
    olivec_rect(oc, -WIDTH/4, -HEIGHT/4, WIDTH/2, HEIGHT/2, BLUE_COLOR);
}

void test_fill_circle(void)
{
    Olivec_Canvas oc = olivec_canvas(actual_pixels, WIDTH, HEIGHT);
    olivec_fill(oc, BACKGROUND_COLOR);
    olivec_circle(oc, 0, 0, WIDTH/2, RED_COLOR);
    olivec_circle(oc, WIDTH/2, HEIGHT/2, WIDTH/4, BLUE_COLOR);
    olivec_circle(oc, WIDTH*3/4, HEIGHT*3/4, -WIDTH/4, GREEN_COLOR);
}

void test_draw_line(void)
{
    Olivec_Canvas oc = olivec_canvas(actual_pixels, WIDTH, HEIGHT);
    olivec_fill(oc, BACKGROUND_COLOR);
    olivec_line(oc, 0, 0, WIDTH, HEIGHT, RED_COLOR);
    olivec_line(oc, WIDTH, 0, 0, HEIGHT, BLUE_COLOR);
    olivec_line(oc, WIDTH/2, 0, WIDTH/2, HEIGHT, GREEN_COLOR);
}

void test_fill_triangle(void)
{
    Olivec_Canvas oc = olivec_canvas(actual_pixels, WIDTH, HEIGHT);

    olivec_fill(oc, BACKGROUND_COLOR);

    {
        int x1 = WIDTH/2,   y1 = HEIGHT/8;
        int x2 = WIDTH/8,   y2 = HEIGHT/2;
        int x3 = WIDTH*7/8, y3 = HEIGHT*7/8;
        olivec_triangle(oc, x1, y1, x2, y2, x3, y3, RED_COLOR);
    }

    {
        int x1 = WIDTH/2,   y1 = HEIGHT*2/8;
        int x2 = WIDTH*2/8, y2 = HEIGHT/2;
        int x3 = WIDTH*6/8, y3 = HEIGHT/2;
        olivec_triangle(oc, x1, y1, x2, y2, x3, y3, GREEN_COLOR);
    }

    {
        int x1 = WIDTH/8,   y1 = HEIGHT/8;
        int x2 = WIDTH/8,   y2 = HEIGHT*3/8;
        int x3 = WIDTH*3/8, y3 = HEIGHT*3/8;
        olivec_triangle(oc, x1, y1, x2, y2, x3, y3, BLUE_COLOR);
    }
}

void test_alpha_blending(void)
{
    Olivec_Canvas oc = olivec_canvas(actual_pixels, WIDTH, HEIGHT);
    olivec_fill(oc, BACKGROUND_COLOR);
    olivec_rect(oc, 0, 0, WIDTH*3/4, HEIGHT*3/4, RED_COLOR);
    olivec_rect(oc, WIDTH-1, HEIGHT-1, -WIDTH*3/4, -HEIGHT*3/4, 0x5520AA20);
    olivec_circle(oc, WIDTH/2, HEIGHT/2, WIDTH/4, 0xBBAA2020);
    olivec_triangle(oc, 0, HEIGHT, WIDTH, HEIGHT, WIDTH/2, 0, 0xBB20AAAA);
}

void test_transparency(void)
{
    Olivec_Canvas oc = olivec_canvas(actual_pixels, WIDTH, HEIGHT);
    olivec_clear(oc);
    olivec_rect(oc, WIDTH*1/6, HEIGHT*1/6, WIDTH, HEIGHT, 0x01FFFFFF);
    olivec_rect(oc, WIDTH*2/6, HEIGHT*2/6, WIDTH, HEIGHT, 0x54FFFFFF);
    olivec_rect(oc, WIDTH*3/6, HEIGHT*3/6, WIDTH, HEIGHT, 0x55FFFFFF);
    olivec_rect(oc, WIDTH*4/6, HEIGHT*4/6, WIDTH, HEIGHT, 0x54FFFFFF);
    olivec_rect(oc, WIDTH*5/6, HEIGHT*5/6, WIDTH, HEIGHT, 0x01FFFFFF);
}

Test_Case test_cases[] = {
    DEFINE_TEST_CASE(test_fill_rect),
    DEFINE_TEST_CASE(test_fill_circle),
    DEFINE_TEST_CASE(test_draw_line),
    DEFINE_TEST_CASE(test_fill_triangle),
    DEFINE_TEST_CASE(test_alpha_blending),
    DEFINE_TEST_CASE(test_transparency),
};
#define TEST_CASES_COUNT (sizeof(test_cases)/sizeof(test_cases[0]))

int main(int argc, char **argv)
{
    assert(argc >= 1);
    const char *program_path = argv[0];
    bool record = argc >= 2 && strcmp(argv[1], "record") == 0;

    for (size_t i = 0; i < TEST_CASES_COUNT; ++i) {
        test_cases[i].generate_actual_pixels();
        if (record) {
            if (!record_test_case(test_cases[i].expected_file_path)) return 1;
        } else {
            if (replay_test_case(program_path, test_cases[i].expected_file_path, test_cases[i].actual_file_path, test_cases[i].diff_file_path) == REPLAY_ERRORED) return 1;
        }
    }
    return 0;
}
