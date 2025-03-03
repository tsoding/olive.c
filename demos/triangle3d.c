#define VC_TRUECOLOR
#include "vc.c"

#define WIDTH 960
#define HEIGHT 720

static uint32_t pixels1[WIDTH*HEIGHT];
static float zbuffer1[WIDTH*HEIGHT];
static uint32_t pixels2[WIDTH*HEIGHT];
static float zbuffer2[WIDTH*HEIGHT];

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

static Vector3 make_vector3(float x, float y, float z)
{
    Vector3 v3;
    v3.x = x;
    v3.y = y;
    v3.z = z;
    return v3;
}

static Vector2 project_3d_2d(Vector3 v3)
{
    return make_vector2(v3.x / v3.z, v3.y / v3.z);
}

static Vector2 project_2d_scr(Vector2 v2)
{
    return make_vector2((v2.x + 1)/2*WIDTH, (1 - (v2.y + 1)/2)*HEIGHT);
}

static float global_time = 1.0;

#define PI 3.14159265359

float sinf(float);
float cosf(float);

Olivec_Canvas vc_render(float dt)
{
    global_time += dt;

    Olivec_Canvas oc1 = olivec_canvas(pixels1, WIDTH, HEIGHT, WIDTH);
    olivec_fill(oc1, 0xFF181818);
    Olivec_Canvas zb1 = olivec_canvas((uint32_t*)zbuffer1, WIDTH, HEIGHT, WIDTH);
    olivec_fill(zb1, 0);

    float z = 1.5;
    float t = 0.75;
    {
        Vector3 v1 = make_vector3(cosf(global_time)*t, -t, z + sinf(global_time)*t);
        Vector3 v2 = make_vector3(cosf(global_time + PI)*t, -t, z + sinf(global_time + PI)*t);
        Vector3 v3 = make_vector3(0, t, z);

        Vector2 p1 = project_2d_scr(project_3d_2d(v1));
        Vector2 p2 = project_2d_scr(project_3d_2d(v2));
        Vector2 p3 = project_2d_scr(project_3d_2d(v3));

        olivec_triangle3z(zb1, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 1.0f/v1.z, 1.0f/v2.z, 1.0f/v3.z);
        olivec_triangle3c(oc1, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 0xFF1818FF, 0xFF18FF18, 0xFFFF1818);
    }

    Olivec_Canvas oc2 = olivec_canvas(pixels2, WIDTH, HEIGHT, WIDTH);
    olivec_fill(oc2, 0xFF181818);
    Olivec_Canvas zb2 = olivec_canvas((uint32_t*)zbuffer2, WIDTH, HEIGHT, WIDTH);
    olivec_fill(zb2, 0);

    {
        Vector3 v1 = make_vector3(cosf(global_time + PI/2)*t, -t, z + sinf(global_time + PI/2)*t);
        Vector3 v2 = make_vector3(cosf(global_time + PI + PI/2)*t, -t, z + sinf(global_time + PI + PI/2)*t);
        Vector3 v3 = make_vector3(0, t, z);

        Vector2 p1 = project_2d_scr(project_3d_2d(v1));
        Vector2 p2 = project_2d_scr(project_3d_2d(v2));
        Vector2 p3 = project_2d_scr(project_3d_2d(v3));

        olivec_triangle3z(zb2, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 1.0f/v1.z, 1.0f/v2.z, 1.0f/v3.z);
        olivec_triangle3c(oc2, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 0xFF1818FF, 0xFF18FF18, 0xFFFF1818);
    }

    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            float z1 = *(float*)&OLIVEC_PIXEL(zb1, x, y);
            float z2 = *(float*)&OLIVEC_PIXEL(zb2, x, y);
            if (z1 < z2) {
                OLIVEC_PIXEL(oc1, x, y) = OLIVEC_PIXEL(oc2, x, y);
                z1 = z2;
            }
            z1 = 1.0f/z1;
            if (z1 >= 1.0) {
                z1 -= 1.0;
                uint32_t v = z1*255;
                if (v > 255) v = 255;
                olivec_blend_color(&OLIVEC_PIXEL(oc1, x, y), (v<<(3*8)));
            }
        }
    }

    return oc1;
}
