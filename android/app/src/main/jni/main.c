#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <string.h>
#include <math.h>

#include <stdio.h>

#define OLIVEC_IMPLEMENTATION

#include "../../../../../olive.c"

/* Canvas */
#define WIDTH 1100
#define HEIGHT 1100

/* Scene */
#define BACKGROUND_COLOR 0xFF181818
#define CIRCLE_RADIUS 100
#define CIRCLE_COLOR 0x99AA2020

#define PI 3.14159265359f

static float triangle_angle = 0.0f;
static float circle_x = WIDTH / 2.0f;
static float circle_y = HEIGHT / 2.0f;
static float circle_dx = 100.0f;
static float circle_dy = 100.0f;

/* Delta time placeholder */
static const float dt = 1.0f / 120.0f;

/* Rotate a point around the center */
static void rotate_point(float *x, float *y) {
    float dx = *x - WIDTH / 2.0f;
    float dy = *y - HEIGHT / 2.0f;
    float mag = sqrtf(dx * dx + dy * dy);
    float dir = atan2f(dy, dx) + triangle_angle;
    *x = cosf(dir) * mag + WIDTH / 2.0f;
    *y = sinf(dir) * mag + HEIGHT / 2.0f;
}



/*
 --------------------  X
 |
 |
 |
 |
 |
 |
 |
 Y
 */

JNIEXPORT void JNICALL
Java_com_me_app_MainActivity_drawLines(JNIEnv *env, jobject thiz, jobject bmp) {
    uint32_t pixels[WIDTH * HEIGHT];
    const Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);

    olivec_fill(oc, 0xFFFFFFFF);

    const int base_y = 50;

    /* E */
//    olivec_line(oc, 200, base_y, 300, base_y, 0xFFFF00FF);
//    olivec_line(oc, 200, base_y * 2, 300, base_y * 2, 0xFF00FFFF);
//    olivec_line(oc, 200, base_y * 3, 300, base_y * 3, 0xFFFF0000);
//    olivec_line(oc, 200, base_y, 200, base_y * 3, 0xFFFF0000);
//
//    /* A */
//    olivec_line(oc, 400, base_y, 350, base_y * 3, 0xFFFF00FF);
//    olivec_line(oc, 400, base_y, 450, base_y * 3, 0xFFFF00FF);
//    olivec_line(oc, 350, base_y * 2, 450, base_y * 2, 0xFFFF00FF);
//



    /* N */
    olivec_line(oc, 600, 600, 600, 800, 0xFF00FF00);

    olivec_line(oc, 650, 800, 600, 600, 0xFF00FF00);

    olivec_line(oc, 650, 800, 650, 600, 0xFF00FF00);




    /* Rectangle */
    //olivec_rect(oc, 400, 200, 100, 150, 0xFF00FF00);

    void *bitmapPixels;
    AndroidBitmap_lockPixels(env, bmp, &bitmapPixels);
    memcpy(bitmapPixels, pixels, sizeof(pixels));
    AndroidBitmap_unlockPixels(env, bmp);
}

JNIEXPORT void JNICALL
Java_com_me_app_MainActivity_drawTriangle(JNIEnv *env, jobject thiz, jobject bmp) {
    uint32_t pixels[WIDTH * HEIGHT];
    const Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);

    olivec_fill(oc, BACKGROUND_COLOR);

    /* Triangle */
    triangle_angle += 0.7f * PI * dt;

    float x1 = WIDTH / 2.0f,       y1 = HEIGHT * 3.0f / 20;
    float x2 = WIDTH * 3.0f / 20,  y2 = HEIGHT / 2.0f;
    float x3 = WIDTH * 17.0f / 20, y3 = HEIGHT * 17.0f / 20;

    rotate_point(&x1, &y1);
    rotate_point(&x2, &y2);
    rotate_point(&x3, &y3);



    olivec_triangle3c(oc, (int) x1, (int) y1, (int) x2, (int) y2, (int) x3, (int) y3,
            0xFF2020FF, 0xFF20FF20, 0xFFFF2020);

    /* Circle */
    float nx = circle_x + circle_dx * dt;
    if (nx - CIRCLE_RADIUS < 0 || nx + CIRCLE_RADIUS >= WIDTH) {
        circle_dx *= -1;
    } else {
        circle_x = nx;
    }

    float ny = circle_y + circle_dy * dt;
    if (ny - CIRCLE_RADIUS < 0 || ny + CIRCLE_RADIUS >= HEIGHT) {
        circle_dy *= -1;
    } else {
        circle_y = ny;
    }

    // == DRAWING THE CIRCLE WITHIN THE TRIANGLE == //
    olivec_circle(oc, (int) circle_x, (int) circle_y, CIRCLE_RADIUS, CIRCLE_COLOR);

    // == CREATE THE BITMAP WITH THE GIVEN POINTER == //
    void *bitmapPixels;
    AndroidBitmap_lockPixels(env, bmp, &bitmapPixels);
    memcpy(bitmapPixels, pixels, sizeof(pixels));
    AndroidBitmap_unlockPixels(env, bmp);
}
