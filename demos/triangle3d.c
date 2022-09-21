#define SCALE_DOWN_FACTOR 20
#include "../demos/vc.c"

#define WIDTH 800
#define HEIGHT 600
uint32_t pixels[WIDTH*HEIGHT];

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

float global_time = 0;

#define PI 3.14159265359

float sinf(float);
float cosf(float);

Olivec_Canvas render(float dt)
{
    global_time += dt;

    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
    olivec_fill(oc, 0xFF181818);

    float z = 1.5;
    {
        Vector2 p1 = project_2d_scr(project_3d_2d(make_vector3(cosf(global_time)*0.5, -0.5, z + sinf(global_time)*0.5)));
        Vector2 p2 = project_2d_scr(project_3d_2d(make_vector3(cosf(global_time + PI)*0.5, -0.5, z + sinf(global_time + PI)*0.5)));
        Vector2 p3 = project_2d_scr(project_3d_2d(make_vector3(0, 0.5, z)));
        olivec_triangle3(oc, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 0xFF1818FF, 0xFF18FF18, 0xFFFF1818);
    }

    {
        Vector2 p1 = project_2d_scr(project_3d_2d(make_vector3(cosf(global_time + PI/2)*0.5, -0.5, z + sinf(global_time + PI/2)*0.5)));
        Vector2 p2 = project_2d_scr(project_3d_2d(make_vector3(cosf(global_time + PI + PI/2)*0.5, -0.5, z + sinf(global_time + PI + PI/2)*0.5)));
        Vector2 p3 = project_2d_scr(project_3d_2d(make_vector3(0, 0.5, z)));
        olivec_triangle3(oc, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, 0xFF1818FF, 0xFF18FF18, 0xFFFF1818);
    }

    
    return oc;
}
