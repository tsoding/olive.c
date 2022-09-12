// This example renders a rotating triangle.
// This idea is that you can take this code and compile it to different platforms with different rendering machanisms:
// native with SDL, WebAssembly with HTML5 canvas, etc.
#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define WIDTH 800
#define HEIGHT 600
#define SCALE_DOWN_FACTOR 20
#define BACKGROUND_COLOR 0xFF181818
#define CIRCLE_RADIUS 100
#define CIRCLE_COLOR 0x99AA2020

static uint32_t pixels[WIDTH*HEIGHT];
static float triangle_angle = 0;
static float circle_x = WIDTH/2;
static float circle_y = HEIGHT/2;
static float circle_dx = 100;
static float circle_dy = 100;

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

#define PI 3.14159265359

static inline void rotate_point(float *x, float *y)
{
    float dx = *x - WIDTH/2;
    float dy = *y - HEIGHT/2;
    float mag = sqrtf(dx*dx + dy*dy);
    float dir = atan2f(dy, dx) + triangle_angle;
    *x = cosf(dir)*mag + WIDTH/2;
    *y = sinf(dir)*mag + HEIGHT/2;
}

void init(void) {}

uint32_t *render(float dt)
{
    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);

    olivec_fill(oc, BACKGROUND_COLOR);

    // Triangle
    {
        triangle_angle += 0.5f*PI*dt;

        float x1 = WIDTH/2, y1 = HEIGHT/8;
        float x2 = WIDTH/8, y2 = HEIGHT/2;
        float x3 = WIDTH*7/8, y3 = HEIGHT*7/8;
        rotate_point(&x1, &y1);
        rotate_point(&x2, &y2);
        rotate_point(&x3, &y3);
        olivec_triangle(oc, x1, y1, x2, y2, x3, y3, 0xFF2020AA);
    }

    // Circle
    {
        float x = circle_x + circle_dx*dt;
        if (x - CIRCLE_RADIUS < 0 || x + CIRCLE_RADIUS >= WIDTH) {
            circle_dx *= -1;
        } else {
            circle_x = x;
        }

        float y = circle_y + circle_dy*dt;
        if (y - CIRCLE_RADIUS < 0 || y + CIRCLE_RADIUS >= HEIGHT) {
            circle_dy *= -1;
        } else {
            circle_y = y;
        }

        olivec_circle(oc, circle_x, circle_y, CIRCLE_RADIUS, CIRCLE_COLOR);
    }

    return pixels;
}

#include "vc.c"
