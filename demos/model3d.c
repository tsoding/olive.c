#include "vc.c"

#define WIDTH 960
#define HEIGHT 720
#define BACKGROUND_COLOR 0xFF181818

#define PI 3.14159265359

float sqrtf(float x);
float atan2f(float y, float x);
float sinf(float x);
float cosf(float x);

static uint32_t pixels[WIDTH*HEIGHT];
static float zbuffer[WIDTH*HEIGHT] = {0};
static float angle = 0;

typedef struct {
    float x, y;
} Vector2;

static Vector2 make_vector2(float x, float y)
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

#define EPSILON 1e-6

static Vector2 project_3d_2d(Vector3 v3)
{
    if (v3.z < 0) v3.z = -v3.z;
    if (v3.z < EPSILON) v3.z += EPSILON;
    return make_vector2(v3.x/v3.z, v3.y/v3.z);
}

static Vector2 project_2d_scr(Vector2 v2)
{
    return make_vector2((v2.x + 1)/2*WIDTH, (1 - (v2.y + 1)/2)*HEIGHT);
}

static Vector3 rotate_y(Vector3 p, float delta_angle)
{
    float angle = atan2f(p.z, p.x) + delta_angle;
    float mag = sqrtf(p.x*p.x + p.z*p.z);
    return make_vector3(cosf(angle)*mag, p.y, sinf(angle)*mag);
}

static Vector3 cross(Vector3 a, Vector3 b) {
    return make_vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

static Vector3 subtract3(Vector3 l, Vector3 r) {
    return make_vector3(l.x - r.x, l.y - r.y, l.z - r.z);
}

static void normalize3(Vector3 *v) {
    float mag_sqr = v->x * v->x + v->y * v->y + v->z * v->z;
    if (mag_sqr < EPSILON*EPSILON) {
        v->x = 0;
        v->y = 0;
        v->z = 0;
        return;
    }
    float mag = sqrtf(mag_sqr);
    v->x /= mag;
    v->y /= mag;
    v->z /= mag;
}

static float dot3(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Olivec_Canvas vc_render(float dt)
{
    angle += 0.25*PI*dt;

    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
    olivec_fill(oc, BACKGROUND_COLOR);
    for (size_t i = 0; i < WIDTH*HEIGHT; ++i) zbuffer[i] = 0;

#ifdef LIGHTING
    Vector3 sun = make_vector3(1, 3, -2);
    normalize3(&sun);
#endif

    for (size_t i = 0; i < faces_count; ++i) {
        int a = faces[i][0];
        int b = faces[i][1];
        int c = faces[i][2];
        Vector3 v1 = rotate_y(make_vector3(vertices[a][0], vertices[a][1], vertices[a][2]), angle);
        Vector3 v2 = rotate_y(make_vector3(vertices[b][0], vertices[b][1], vertices[b][2]), angle);
        Vector3 v3 = rotate_y(make_vector3(vertices[c][0], vertices[c][1], vertices[c][2]), angle);
        v1.z += 1.5; v2.z += 1.5; v3.z += 1.5;
        Vector2 p1 = project_2d_scr(project_3d_2d(v1));
        Vector2 p2 = project_2d_scr(project_3d_2d(v2));
        Vector2 p3 = project_2d_scr(project_3d_2d(v3));

#ifdef LIGHTING
        Vector3 normal = cross(subtract3(v2, v1), subtract3(v3, v1));
        normalize3(&normal);

        float diff = (dot3(normal, sun) + 1.0f) / 2.0f;
        int dq = (int)(diff * 255);
#endif

#ifdef BACKFACE_CULLING
        if ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) <= 0) continue;
#endif
#ifdef FRONTFACE_CULLING
        if ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x) >= 0) continue;
#endif

        int x1 = p1.x;
        int x2 = p2.x;
        int x3 = p3.x;
        int y1 = p1.y;
        int y2 = p2.y;
        int y3 = p3.y;
        int lx, hx, ly, hy;
        if (olivec_normalize_triangle(oc.width, oc.height, x1, y1, x2, y2, x3, y3, &lx, &hx, &ly, &hy)) {
            for (int y = ly; y <= hy; ++y) {
                bool has_entered = false;
            
                for (int x = lx; x <= hx; ++x) {
                    int u1, u2, det;
                    if (olivec_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
                        has_entered = true;
                        int u3 = det - u1 - u2;
                        float z = 1/v1.z*u1/det + 1/v2.z*u2/det + 1/v3.z*u3/det;
                        float near = 0.1f;
                        float far = 5.0f;
                        if (1.0f/far < z && z < 1.0f/near && z > zbuffer[y*WIDTH + x]) {
                            zbuffer[y*WIDTH + x] = z;
                            OLIVEC_PIXEL(oc, x, y) = mix_colors3(0xFF1818FF, 0xFF18FF18, 0xFFFF1818, u1, u2, det);
#ifdef LIGHTING
                            olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), ((255 - dq)<<(3*8)));
#endif

                            z = 1.0f/z;
                            if (z >= 1.0) {
                                z -= 1.0;
                                uint32_t v = z*255;
                                if (v > 255) v = 255;
                                olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), (v<<(3*8)));
                            }
                        }
                    } else if (has_entered) break;
                }
            }
        }
    }

    return oc;
}
