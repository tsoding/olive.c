//////// Start of inlined file: tigr_amalgamated.c ////////


#include "tigr.h"

//////// Start of inlined file: tigr_internal.h ////////

// can't use pragma once here because this file probably will endup in .c
#ifndef __TIGR_INTERNAL_H__
#define __TIGR_INTERNAL_H__

#define _CRT_SECURE_NO_WARNINGS NOPE

// Graphics configuration.
#ifndef TIGR_HEADLESS
#define TIGR_GAPI_GL
#endif

// Creates a new bitmap, with extra payload bytes.
Tigr* tigrBitmap2(int w, int h, int extra);

// Resizes an existing bitmap.
void tigrResize(Tigr* bmp, int w, int h);

// Calculates the biggest scale that a bitmap can fit into an area at.
int tigrCalcScale(int bmpW, int bmpH, int areaW, int areaH);

// Calculates a new scale, taking minimum-scale flags into account.
int tigrEnforceScale(int scale, int flags);

// Calculates the correct position for a bitmap to fit into a window.
void tigrPosition(Tigr* bmp, int scale, int windowW, int windowH, int out[4]);

// ----------------------------------------------------------
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if !defined(TIGR_HEADLESS) && __linux__ && !__ANDROID__
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define __IOS__ 1
#else
#define __MACOS__ 1
#endif
#endif

#ifdef TIGR_GAPI_GL
#if __MACOS__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif
#ifdef _WIN32
#include <GL/gl.h>
#endif
#if __linux__ && !__ANDROID__
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
#endif
#if __ANDROID__
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif
#if __IOS__
#define GLES_SILENCE_DEPRECATION
#include <OpenGLES/ES3/gl.h>
#endif

typedef struct {
#ifdef _WIN32
    HGLRC hglrc;
    HDC dc;
#endif
#ifdef __APPLE__
    void* glContext;
#endif
    GLuint tex[2];
    GLuint vao;
    GLuint program;
    GLuint uniform_projection;
    GLuint uniform_model;
    GLuint uniform_parameters;
    int gl_legacy;
    int gl_user_opengl_rendering;
} GLStuff;
#endif

#define MAX_TOUCH_POINTS 10

typedef struct {
    int shown, closed;
#ifdef TIGR_GAPI_GL
    GLStuff gl;
#ifdef _WIN32
    wchar_t* wtitle;
    DWORD dwStyle;
    RECT oldPos;
#endif
#ifdef __linux__
#if __ANDROID__
    EGLContext context;
#else
    Display* dpy;
    Window win;
    GLXContext glc;
    XIC ic;
#endif  // __ANDROID__
#endif  // __linux__
#endif  // TIGR_GAPI_GL

    Tigr* widgets;
    int widgetsWanted;
    unsigned char widgetAlpha;
    float widgetsScale;

    float p1, p2, p3, p4;

    int flags;
    int scale;
    int pos[4];
    int lastChar;
    char keys[256], prev[256];
#if defined(__ANDROID__)
    char released[256];
#endif  // __ANDROID__
#if defined(__MACOS__)
    int mouseInView;
    int mouseButtons;
#endif  // __MACOS__
#if defined(__linux__) || defined(__IOS__)
    int mouseButtons;
    int mouseX;
    int mouseY;
#endif  // __linux__ __IOS__
#if defined(__ANDROID__) || defined(__IOS__)
    int numTouchPoints;
    TigrTouchPoint touchPoints[MAX_TOUCH_POINTS];
#endif  // __ANDROID__ __IOS__
} TigrInternal;
// ----------------------------------------------------------

TigrInternal* tigrInternal(Tigr* bmp);

void tigrGAPICreate(Tigr* bmp);
void tigrGAPIDestroy(Tigr* bmp);
int tigrGAPIBegin(Tigr* bmp);
int tigrGAPIEnd(Tigr* bmp);
void tigrGAPIPresent(Tigr* bmp, int w, int h);

#endif

//////// End of inlined file: tigr_internal.h ////////

//////// Start of inlined file: tigr_upscale_gl_vs.h ////////

#ifndef __TIGR_UPSCALE_GL_VS_H__
#define __TIGR_UPSCALE_GL_VS_H__

//////// Start of inlined file: tigr_glsl_hdr.h ////////

#ifndef __TIGR_GLSL_HDR_H__
#define __TIGR_GLSL_HDR_H__

#if __ANDROID__ || __IOS__
#define GLSL_VERSION_HEADER \
    "#version 300 es\n"     \
    "precision mediump float;\n"
#else
#define GLSL_VERSION_HEADER "#version 330 core\n"
#endif

#endif  // __TIGR_GLSL_HDR_H__

//////// End of inlined file: tigr_glsl_hdr.h ////////


// clang-format off
const char tigr_upscale_gl_vs[] = {
    GLSL_VERSION_HEADER
    "layout (location = 0) in vec2 pos_in;"
    "layout (location = 1) in vec2 uv_in;"
    "out vec2 uv;"
    "uniform mat4 model;"
    "uniform mat4 projection;"
    "void main()"
    "{"
    "   uv = uv_in;"
    "   gl_Position = projection * model * vec4(pos_in, 0.0, 1.0);"
    "}"
};
// clang-format on

const int tigr_upscale_gl_vs_size = (int)sizeof(tigr_upscale_gl_vs) - 1;

#endif

//////// End of inlined file: tigr_upscale_gl_vs.h ////////

//////// Start of inlined file: tigr_upscale_gl_fs.h ////////

#ifndef __TIGR_UPSCALE_GL_FS_H__
#define __TIGR_UPSCALE_GL_FS_H__

//#include "tigr_glsl_hdr.h"

// clang-format off
const char tigr_upscale_gl_fs[] = {
    GLSL_VERSION_HEADER
    "in vec2 uv;"
    "out vec4 color;"
    "uniform sampler2D image;"
    "uniform vec4 parameters;"
    "void fxShader(out vec4 color, in vec2 coord);"
    "void main()"
    "{"
    "   fxShader(color, uv);"
    "}\n"
};
// clang-format on

const int tigr_upscale_gl_fs_size = (int)sizeof(tigr_upscale_gl_fs) - 1;

// clang-format off
const char tigr_default_fx_gl_fs[] = {
    "void fxShader(out vec4 color, in vec2 uv) {"
    "   vec2 tex_size = vec2(textureSize(image, 0));"
    "   vec2 uv_blur = mix(floor(uv * tex_size) + 0.5, uv * tex_size, parameters.xy) / tex_size;"
    "   vec4 c = texture(image, uv_blur);"
    "   c.rgb *= mix(0.5, 1.0 - fract(uv.y * tex_size.y), parameters.z) * 2.0; //scanline\n"
    "   c = mix(vec4(0.5), c, parameters.w); //contrast\n"
    "   color = c;"
    "}"
};
// clang-format on

const int tigr_default_fx_gl_fs_size = (int)sizeof(tigr_default_fx_gl_fs) - 1;

#endif

//////// End of inlined file: tigr_upscale_gl_fs.h ////////


//////// Start of inlined file: tigr_bitmaps.c ////////

//#include "tigr_internal.h"
#include <stdlib.h>
#include <string.h>

// Expands 0-255 into 0-256
#define EXPAND(X) ((X) + ((X) > 0))

#define CLIP0(CX, X, X2, W) \
    if (X < CX) {           \
        int D = CX - X;     \
        W -= D;             \
        X2 += D;            \
        X += D;             \
    }
#define CLIP1(X, DW, W) \
    if (X + W > DW)     \
        W = DW - X;
#define CLIP()                  \
    CLIP0(dst->cx, dx, sx, w);  \
    CLIP0(dst->cy, dy, sy, h);  \
    CLIP0(0, sx, dx, w);        \
    CLIP0(0, sy, dy, h);        \
    CLIP1(dx, dst->cx + cw, w); \
    CLIP1(dy, dst->cy + ch, h); \
    CLIP1(sx, src->w, w);       \
    CLIP1(sy, src->h, h);       \
    if (w <= 0 || h <= 0)       \
    return

Tigr* tigrBitmap2(int w, int h, int extra) {
    Tigr* tigr = (Tigr*)calloc(1, sizeof(Tigr) + extra);
    tigr->w = w;
    tigr->h = h;
    tigr->cw = -1;
    tigr->ch = -1;
    tigr->pix = (TPixel*)calloc(w * h, sizeof(TPixel));
    tigr->blitMode = TIGR_BLEND_ALPHA;
    return tigr;
}

Tigr* tigrBitmap(int w, int h) {
    return tigrBitmap2(w, h, 0);
}

#ifdef TIGR_HEADLESS
void tigrFree(Tigr* bmp) {
    free(bmp->pix);
    free(bmp);
}
#endif // TIGR_HEADLESS


void tigrResize(Tigr* bmp, int w, int h) {
    if (bmp->w == w && bmp->h == h) {
        return;
    }

    int y, cw, ch;
    TPixel* newpix = (TPixel*)calloc(w * h, sizeof(TPixel));
    cw = (w < bmp->w) ? w : bmp->w;
    ch = (h < bmp->h) ? h : bmp->h;

    // Copy any old data across.
    for (y = 0; y < ch; y++)
        memcpy(newpix + y * w, bmp->pix + y * bmp->w, cw * sizeof(TPixel));

    free(bmp->pix);
    bmp->pix = newpix;
    bmp->w = w;
    bmp->h = h;
}

int tigrCalcScale(int bmpW, int bmpH, int areaW, int areaH) {
    // We want it as big as possible in the window, but still
    // maintaining the correct aspect ratio, and always
    // having an integer pixel size.
    int scale = 0;
    for (;;) {
        scale++;
        if (bmpW * scale > areaW || bmpH * scale > areaH) {
            scale--;
            break;
        }
    }
    return (scale > 1) ? scale : 1;
}

int tigrEnforceScale(int scale, int flags) {
    if ((flags & TIGR_4X) && scale < 4)
        scale = 4;
    if ((flags & TIGR_3X) && scale < 3)
        scale = 3;
    if ((flags & TIGR_2X) && scale < 2)
        scale = 2;
    return scale;
}

void tigrPosition(Tigr* bmp, int scale, int windowW, int windowH, int out[4]) {
    // Center the image on screen at this scale.
    out[0] = (windowW - bmp->w * scale) / 2;
    out[1] = (windowH - bmp->h * scale) / 2;
    out[2] = out[0] + bmp->w * scale;
    out[3] = out[1] + bmp->h * scale;
}

void tigrClear(Tigr* bmp, TPixel color) {
    int count = bmp->w * bmp->h;
    int n;
    for (n = 0; n < count; n++)
        bmp->pix[n] = color;
}

void tigrFill(Tigr* bmp, int x, int y, int w, int h, TPixel color) {
    TPixel* td;
    int dt, i;

    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > bmp->w) {
        w = bmp->w - x;
    }
    if (y + h > bmp->h) {
        h = bmp->h - y;
    }
    if (w <= 0 || h <= 0)
        return;

    td = &bmp->pix[y * bmp->w + x];
    dt = bmp->w;
    do {
        for (i = 0; i < w; i++)
            td[i] = color;
        td += dt;
    } while (--h);
}

void tigrLine(Tigr* bmp, int x0, int y0, int x1, int y1, TPixel color) {
    int sx, sy, dx, dy, err, e2;
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    if (x0 < x1)
        sx = 1;
    else
        sx = -1;
    if (y0 < y1)
        sy = 1;
    else
        sy = -1;
    err = dx - dy;

    do {
        tigrPlot(bmp, x0, y0, color);
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    } while (x0 != x1 || y0 != y1);
}

void tigrFillRect(Tigr* bmp, int x, int y, int w, int h, TPixel color) {
    x += 1;
    y += 1;
    w -= 2;
    h -= 2;

    int cx = bmp->cx;
    int cy = bmp->cy;
    int cw = bmp->cw >= 0 ? bmp->cw : bmp->w;
    int ch = bmp->ch >= 0 ? bmp->ch : bmp->h;

    if (x < cx) {
        w += (x - cx);
        x = cx;
    }
    if (y < cy) {
        h += (y - cy);
        y = cy;
    }
    if (x + w > cx + cw) {
        w -= (x + w) - (cx + cw);
    }
    if (y + h > cy + ch) {
        h -= (y + h) - (cy + ch);
    }
    if (w <= 0 || h <= 0)
        return;

    TPixel* td = &bmp->pix[y * bmp->w + x];
    int dt = bmp->w;
    int xa = EXPAND(color.a);
    int a = xa * xa;

    do {
        for (int i = 0; i < w; i++) {
            td[i].r += (unsigned char)((color.r - td[i].r) * a >> 16);
            td[i].g += (unsigned char)((color.g - td[i].g) * a >> 16);
            td[i].b += (unsigned char)((color.b - td[i].b) * a >> 16);
            td[i].a += (bmp->blitMode) * (unsigned char)((color.a - td[i].a) * a >> 16);
        }
        td += dt;
    } while (--h);
}

void tigrRect(Tigr* bmp, int x, int y, int w, int h, TPixel color) {
    int x1, y1;
    if (w <= 0 || h <= 0) {
        return;
    }

    if (w == 1) {
        tigrLine(bmp, x, y, x, y + h, color);
    } else if (h == 1) {
        tigrLine(bmp, x, y, x + w, y, color);
    } else {
        x1 = x + w - 1;
        y1 = y + h - 1;
        tigrLine(bmp, x, y, x1, y, color);
        tigrLine(bmp, x1, y, x1, y1, color);
        tigrLine(bmp, x1, y1, x, y1, color);
        tigrLine(bmp, x, y1, x, y, color);
    }
}

void tigrFillCircle(Tigr* bmp, int x0, int y0, int r, TPixel color) {
    if (r <= 0) {
        return;
    }

    int E = 1 - r;
    int dx = 0;
    int dy = -2 * r;
    int x = 0;
    int y = r;

    tigrLine(bmp, x0 - r + 1, y0, x0 + r, y0, color);

    while (x < y - 1) {
        x++;

        if (E >= 0) {
            y--;
            dy += 2;
            E += dy;
            tigrLine(bmp, x0 - x + 1, y0 + y, x0 + x, y0 + y, color);
            tigrLine(bmp, x0 - x + 1, y0 - y, x0 + x, y0 - y, color);
        }

        dx += 2;
        E += dx + 1;

        if (x != y) {
            tigrLine(bmp, x0 - y + 1, y0 + x, x0 + y, y0 + x, color);
            tigrLine(bmp, x0 - y + 1, y0 - x, x0 + y, y0 - x, color);
        }
    }
}

void tigrCircle(Tigr* bmp, int x0, int y0, int r, TPixel color) {
    int E = 1 - r;
    int dx = 0;
    int dy = -2 * r;
    int x = 0;
    int y = r;

    tigrPlot(bmp, x0, y0 + r, color);
    tigrPlot(bmp, x0, y0 - r, color);
    tigrPlot(bmp, x0 + r, y0, color);
    tigrPlot(bmp, x0 - r, y0, color);

    while (x < y - 1) {
        x++;

        if (E >= 0) {
            y--;
            dy += 2;
            E += dy;
        }

        dx += 2;
        E += dx + 1;

        tigrPlot(bmp, x0 + x, y0 + y, color);
        tigrPlot(bmp, x0 - x, y0 + y, color);
        tigrPlot(bmp, x0 + x, y0 - y, color);
        tigrPlot(bmp, x0 - x, y0 - y, color);

        if (x != y) {
            tigrPlot(bmp, x0 + y, y0 + x, color);
            tigrPlot(bmp, x0 - y, y0 + x, color);
            tigrPlot(bmp, x0 + y, y0 - x, color);
            tigrPlot(bmp, x0 - y, y0 - x, color);
        }
    }
}

TPixel tigrGet(Tigr* bmp, int x, int y) {
    TPixel empty = { 0, 0, 0, 0 };
    if (x >= 0 && y >= 0 && x < bmp->w && y < bmp->h)
        return bmp->pix[y * bmp->w + x];
    return empty;
}

void tigrPlot(Tigr* bmp, int x, int y, TPixel pix) {
    int xa, i, a;

    int cx = bmp->cx;
    int cy = bmp->cy;
    int cw = bmp->cw >= 0 ? bmp->cw : bmp->w;
    int ch = bmp->ch >= 0 ? bmp->ch : bmp->h;

    if (x >= cx && y >= cy && x < cx + cw && y < cy + ch) {
        xa = EXPAND(pix.a);
        a = xa * xa;
        i = y * bmp->w + x;

        bmp->pix[i].r += (unsigned char)((pix.r - bmp->pix[i].r) * a >> 16);
        bmp->pix[i].g += (unsigned char)((pix.g - bmp->pix[i].g) * a >> 16);
        bmp->pix[i].b += (unsigned char)((pix.b - bmp->pix[i].b) * a >> 16);
        bmp->pix[i].a += (bmp->blitMode) * (unsigned char)((pix.a - bmp->pix[i].a) * a >> 16);
    }
}

void tigrClip(Tigr* bmp, int cx, int cy, int cw, int ch) {
    bmp->cx = cx;
    bmp->cy = cy;
    bmp->cw = cw;
    bmp->ch = ch;
}

void tigrBlit(Tigr* dst, Tigr* src, int dx, int dy, int sx, int sy, int w, int h) {
    int cw = dst->cw >= 0 ? dst->cw : dst->w;
    int ch = dst->ch >= 0 ? dst->ch : dst->h;

    CLIP();

    TPixel* ts = &src->pix[sy * src->w + sx];
    TPixel* td = &dst->pix[dy * dst->w + dx];
    int st = src->w;
    int dt = dst->w;
    do {
        memcpy(td, ts, w * sizeof(TPixel));
        ts += st;
        td += dt;
    } while (--h);
}

void tigrBlitTint(Tigr* dst, Tigr* src, int dx, int dy, int sx, int sy, int w, int h, TPixel tint) {
    int cw = dst->cw >= 0 ? dst->cw : dst->w;
    int ch = dst->ch >= 0 ? dst->ch : dst->h;

    CLIP();

    int xr = EXPAND(tint.r);
    int xg = EXPAND(tint.g);
    int xb = EXPAND(tint.b);
    int xa = EXPAND(tint.a);

    TPixel* ts = &src->pix[sy * src->w + sx];
    TPixel* td = &dst->pix[dy * dst->w + dx];
    int st = src->w;
    int dt = dst->w;
    do {
        for (int x = 0; x < w; x++) {
            unsigned r = (xr * ts[x].r) >> 8;
            unsigned g = (xg * ts[x].g) >> 8;
            unsigned b = (xb * ts[x].b) >> 8;
            unsigned a = xa * EXPAND(ts[x].a);
            td[x].r += (unsigned char)((r - td[x].r) * a >> 16);
            td[x].g += (unsigned char)((g - td[x].g) * a >> 16);
            td[x].b += (unsigned char)((b - td[x].b) * a >> 16);
            td[x].a += (dst->blitMode) * (unsigned char)((ts[x].a - td[x].a) * a >> 16);
        }
        ts += st;
        td += dt;
    } while (--h);
}

void tigrBlitAlpha(Tigr* dst, Tigr* src, int dx, int dy, int sx, int sy, int w, int h, float alpha) {
    alpha = (alpha < 0) ? 0 : (alpha > 1 ? 1 : alpha);
    tigrBlitTint(dst, src, dx, dy, sx, sy, w, h, tigrRGBA(0xff, 0xff, 0xff, (unsigned char)(alpha * 255)));
}

void tigrBlitMode(Tigr* dst, int mode) {
    dst->blitMode = mode;
}

#undef CLIP0
#undef CLIP1
#undef CLIP

//////// End of inlined file: tigr_bitmaps.c ////////

//////// Start of inlined file: tigr_loadpng.c ////////

//#include "tigr_internal.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    const unsigned char *p, *end;
} PNG;

static unsigned get32(const unsigned char* v) {
    return (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
}

static const unsigned char* find(PNG* png, const char* chunk, unsigned minlen) {
    const unsigned char* start;
    while (png->p < png->end) {
        unsigned len = get32(png->p + 0);
        start = png->p;
        png->p += len + 12;
        if (memcmp(start + 4, chunk, 4) == 0 && len >= minlen && png->p <= png->end)
            return start + 8;
    }

    return NULL;
}

static unsigned char paeth(unsigned char a, unsigned char b, unsigned char c) {
    int p = a + b - c;
    int pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    return (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;
}

static int rowBytes(int w, int bipp) {
    int rowBits = w * bipp;
    return rowBits / 8 + ((rowBits % 8) ? 1 : 0);
}

static int unfilter(int w, int h, int bipp, unsigned char* raw) {
    int len = rowBytes(w, bipp);
    int bpp = rowBytes(1, bipp);
    int x, y;
    unsigned char* first = (unsigned char*)malloc(len + 1);
    memset(first, 0, len + 1);
    unsigned char* prev = first;
    for (y = 0; y < h; y++, prev = raw, raw += len) {
#define LOOP(A, B)            \
    for (x = 0; x < bpp; x++) \
        raw[x] += A;          \
    for (; x < len; x++)      \
        raw[x] += B;          \
    break
        switch (*raw++) {
            case 0:
                break;
            case 1:
                LOOP(0, raw[x - bpp]);
            case 2:
                LOOP(prev[x], prev[x]);
            case 3:
                LOOP(prev[x] / 2, (raw[x - bpp] + prev[x]) / 2);
            case 4:
                LOOP(prev[x], paeth(raw[x - bpp], prev[x], prev[x - bpp]));
            default:
                return 0;
        }
#undef LOOP
    }
    free(first);
    return 1;
}

static void convert(int bypp, int w, int h, const unsigned char* src, TPixel* dest, const unsigned char* trns) {
    int x, y;
    for (y = 0; y < h; y++) {
        src++;  // skip filter byte
        for (x = 0; x < w; x++, src += bypp) {
            switch (bypp) {
                case 1: {
                    unsigned char c = src[0];
                    if (trns && c == *trns) {
                        *dest++ = tigrRGBA(c, c, c, 0);
                        break;
                    } else {
                        *dest++ = tigrRGB(c, c, c);
                        break;
                    }
                }
                case 2:
                    *dest++ = tigrRGBA(src[0], src[0], src[0], src[1]);
                    break;
                case 3: {
                    unsigned char r = src[0];
                    unsigned char g = src[1];
                    unsigned char b = src[2];
                    if (trns && trns[1] == r && trns[3] == g && trns[5] == b) {
                        *dest++ = tigrRGBA(r, g, b, 0);
                        break;
                    } else {
                        *dest++ = tigrRGB(r, g, b);
                        break;
                    }
                }
                case 4:
                    *dest++ = tigrRGBA(src[0], src[1], src[2], src[3]);
                    break;
            }
        }
    }
}

static void depalette(int w,
                      int h,
                      unsigned char* src,
                      TPixel* dest,
                      int bipp,
                      const unsigned char* plte,
                      const unsigned char* trns,
                      int trnsSize) {
    int x, y, c;
    unsigned char alpha;
    int mask = 0, len = 0;

    switch (bipp) {
        case 4:
            mask = 15;
            len = 1;
            break;
        case 2:
            mask = 3;
            len = 3;
            break;
        case 1:
            mask = 1;
            len = 7;
    }

    for (y = 0; y < h; y++) {
        src++;  // skip filter byte
        for (x = 0; x < w; x++) {
            if (bipp == 8) {
                c = *src++;
            } else {
                int pos = x & len;
                c = (src[0] >> ((len - pos) * bipp)) & mask;
                if (pos == len) {
                    src++;
                }
            }
            alpha = 255;
            if (c < trnsSize) {
                alpha = trns[c];
            }
            *dest++ = tigrRGBA(plte[c * 3 + 0], plte[c * 3 + 1], plte[c * 3 + 2], alpha);
        }
    }
}

#define FAIL()          \
    {                   \
        errno = EINVAL; \
        goto err;       \
    }
#define CHECK(X) \
    if (!(X))    \
    FAIL()

static int outsize(Tigr* bmp, int bipp) {
    return (rowBytes(bmp->w, bipp) + 1) * bmp->h;
}

static Tigr* tigrLoadPng(PNG* png) {
    const unsigned char *ihdr, *idat, *plte, *trns, *first;
    int trnsSize = 0;
    int depth, ctype, bipp;
    int datalen = 0;
    unsigned char *data = NULL, *out;
    Tigr* bmp = NULL;

    CHECK(memcmp(png->p, "\211PNG\r\n\032\n", 8) == 0);  // PNG signature
    png->p += 8;
    first = png->p;

    // Read IHDR
    ihdr = find(png, "IHDR", 13);
    CHECK(ihdr);
    depth = ihdr[8];
    ctype = ihdr[9];
    switch (ctype) {
        case 0:
            bipp = depth;
            break;  // greyscale
        case 2:
            bipp = 3 * depth;
            break;  // RGB
        case 3:
            bipp = depth;
            break;  // paletted
        case 4:
            bipp = 2 * depth;
            break;  // grey+alpha
        case 6:
            bipp = 4 * depth;
            break;  // RGBA
        default:
            FAIL();
    }

    // Allocate bitmap (+1 width to save room for stupid PNG filter bytes)
    bmp = tigrBitmap(get32(ihdr + 0) + 1, get32(ihdr + 4));
    CHECK(bmp);
    bmp->w--;

    // We support 8-bit color components and 1, 2, 4 and 8 bit palette formats.
    // No interlacing, or wacky filter types.
    CHECK((depth != 16) && ihdr[10] == 0 && ihdr[11] == 0 && ihdr[12] == 0);

    // Join IDAT chunks.
    for (idat = find(png, "IDAT", 0); idat; idat = find(png, "IDAT", 0)) {
        unsigned len = get32(idat - 8);
        data = (unsigned char*)realloc(data, datalen + len);
        if (!data)
            break;

        memcpy(data + datalen, idat, len);
        datalen += len;
    }

    // Find palette.
    png->p = first;
    plte = find(png, "PLTE", 0);

    // Find transparency info.
    png->p = first;
    trns = find(png, "tRNS", 0);
    if (trns) {
        trnsSize = get32(trns - 8);
    }

    CHECK(data && datalen >= 6);
    CHECK((data[0] & 0x0f) == 0x08     // compression method (RFC 1950)
          && (data[0] & 0xf0) <= 0x70  // window size
          && (data[1] & 0x20) == 0);   // preset dictionary present

    out = (unsigned char*)bmp->pix + outsize(bmp, 32) - outsize(bmp, bipp);
    CHECK(tigrInflate(out, outsize(bmp, bipp), data + 2, datalen - 6));
    CHECK(unfilter(bmp->w, bmp->h, bipp, out));

    if (ctype == 3) {
        CHECK(plte);
        depalette(bmp->w, bmp->h, out, bmp->pix, bipp, plte, trns, trnsSize);
    } else {
        CHECK(bipp % 8 == 0);
        convert(bipp / 8, bmp->w, bmp->h, out, bmp->pix, trns);
    }

    free(data);
    return bmp;

err:
    if (data)
        free(data);
    if (bmp)
        tigrFree(bmp);
    return NULL;
}

#undef CHECK
#undef FAIL

Tigr* tigrLoadImageMem(const void* data, int length) {
    PNG png;
    png.p = (unsigned char*)data;
    png.end = (unsigned char*)data + length;
    return tigrLoadPng(&png);
}

Tigr* tigrLoadImage(const char* fileName) {
    int len;
    void* data;
    Tigr* bmp;

    data = tigrReadFile(fileName, &len);
    if (!data)
        return NULL;

    bmp = tigrLoadImageMem(data, len);
    free(data);
    return bmp;
}

//////// End of inlined file: tigr_loadpng.c ////////

//////// Start of inlined file: tigr_savepng.c ////////

//#include "tigr_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    unsigned crc, adler, bits, prev, runlen;
    FILE* out;
    unsigned crcTable[256];
} Save;

static const unsigned crctable[16] = { 0,          0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4,
                                       0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
                                       0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };

static void put(Save* s, unsigned v) {
    fputc(v, s->out);
    s->crc = (s->crc >> 4) ^ crctable[(s->crc & 15) ^ (v & 15)];
    s->crc = (s->crc >> 4) ^ crctable[(s->crc & 15) ^ (v >> 4)];
}

static void updateAdler(Save* s, unsigned v) {
    unsigned s1 = s->adler & 0xffff, s2 = (s->adler >> 16) & 0xffff;
    s1 = (s1 + v) % 65521;
    s2 = (s2 + s1) % 65521;
    s->adler = (s2 << 16) + s1;
}

static void put32(Save* s, unsigned v) {
    put(s, (v >> 24) & 0xff);
    put(s, (v >> 16) & 0xff);
    put(s, (v >> 8) & 0xff);
    put(s, v & 0xff);
}

void putbits(Save* s, unsigned data, unsigned bitcount) {
    while (bitcount--) {
        unsigned prev = s->bits;
        s->bits = (s->bits >> 1) | ((data & 1) << 7);
        data >>= 1;
        if (prev & 1) {
            put(s, s->bits);
            s->bits = 0x80;
        }
    }
}

void putbitsr(Save* s, unsigned data, unsigned bitcount) {
    while (bitcount--)
        putbits(s, data >> bitcount, 1);
}

static void begin(Save* s, const char* id, unsigned len) {
    put32(s, len);
    s->crc = 0xffffffff;
    put(s, id[0]);
    put(s, id[1]);
    put(s, id[2]);
    put(s, id[3]);
}

static void literal(Save* s, unsigned v) {
    // Encode a literal/length using the built-in tables.
    // Could do better with a custom table but whatever.
    if (v < 144)
        putbitsr(s, 0x030 + v - 0, 8);
    else if (v < 256)
        putbitsr(s, 0x190 + v - 144, 9);
    else if (v < 280)
        putbitsr(s, 0x000 + v - 256, 7);
    else
        putbitsr(s, 0x0c0 + v - 280, 8);
}

static void encodelen(Save* s, unsigned code, unsigned bits, unsigned len) {
    literal(s, code + (len >> bits));
    putbits(s, len, bits);
    putbits(s, 0, 5);
}

static void endrun(Save* s) {
    s->runlen--;
    literal(s, s->prev);

    if (s->runlen >= 67)
        encodelen(s, 277, 4, s->runlen - 67);
    else if (s->runlen >= 35)
        encodelen(s, 273, 3, s->runlen - 35);
    else if (s->runlen >= 19)
        encodelen(s, 269, 2, s->runlen - 19);
    else if (s->runlen >= 11)
        encodelen(s, 265, 1, s->runlen - 11);
    else if (s->runlen >= 3)
        encodelen(s, 257, 0, s->runlen - 3);
    else
        while (s->runlen--)
            literal(s, s->prev);
}

static void encodeByte(Save* s, unsigned char v) {
    updateAdler(s, v);

    // Simple RLE compression. We could do better by doing a search
    // to find matches, but this works pretty well TBH.
    if (s->prev == v && s->runlen < 115) {
        s->runlen++;
    } else {
        if (s->runlen)
            endrun(s);

        s->prev = v;
        s->runlen = 1;
    }
}

static void savePngHeader(Save* s, Tigr* bmp) {
    fwrite("\211PNG\r\n\032\n", 8, 1, s->out);
    begin(s, "IHDR", 13);
    put32(s, bmp->w);
    put32(s, bmp->h);
    put(s, 8);  // bit depth
    put(s, 6);  // RGBA
    put(s, 0);  // compression (deflate)
    put(s, 0);  // filter (standard)
    put(s, 0);  // interlace off
    put32(s, ~s->crc);
}

static long savePngData(Save* s, Tigr* bmp, long dataPos) {
    int x, y;
    long dataSize;
    begin(s, "IDAT", 0);
    put(s, 0x08);      // zlib compression method
    put(s, 0x1d);      // zlib compression flags
    putbits(s, 3, 3);  // zlib last block + fixed dictionary
    for (y = 0; y < bmp->h; y++) {
        TPixel* row = &bmp->pix[y * bmp->w];
        TPixel prev = tigrRGBA(0, 0, 0, 0);

        encodeByte(s, 1);  // sub filter
        for (x = 0; x < bmp->w; x++) {
            encodeByte(s, row[x].r - prev.r);
            encodeByte(s, row[x].g - prev.g);
            encodeByte(s, row[x].b - prev.b);
            encodeByte(s, row[x].a - prev.a);
            prev = row[x];
        }
    }
    endrun(s);
    literal(s, 256);  // terminator
    while (s->bits != 0x80)
        putbits(s, 0, 1);
    put32(s, s->adler);
    dataSize = (ftell(s->out) - dataPos) - 8;
    put32(s, ~s->crc);
    return dataSize;
}

int tigrSaveImage(const char* fileName, Tigr* bmp) {
    Save s;
    long dataPos, dataSize, err;

    // TODO - unicode?
    FILE* out = fopen(fileName, "wb");
    if (!out)
        return 1;

    s.out = out;
    s.adler = 1;
    s.bits = 0x80;
    s.prev = 0xffff;
    s.runlen = 0;

    savePngHeader(&s, bmp);
    dataPos = ftell(s.out);
    dataSize = savePngData(&s, bmp, dataPos);

    // End chunk.
    begin(&s, "IEND", 0);
    put32(&s, ~s.crc);

    // Write back payload size.
    fseek(out, dataPos, SEEK_SET);
    put32(&s, dataSize);

    err = ferror(out);
    fclose(out);
    return !err;
}

//////// End of inlined file: tigr_savepng.c ////////

//////// Start of inlined file: tigr_inflate.c ////////

//#include "tigr_internal.h"
#include <stdlib.h>
#include <setjmp.h>

typedef struct {
    unsigned bits, count;
    const unsigned char *in, *inend;
    unsigned char *out, *outend;
    jmp_buf jmp;
    unsigned litcodes[288], distcodes[32], lencodes[19];
    int tlit, tdist, tlen;
} State;

#define FAIL() longjmp(s->jmp, 1)
#define CHECK(X) \
    if (!(X))    \
    FAIL()

// Built-in DEFLATE standard tables.
static char order[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
static char lenBits[29 + 2] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                                3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };
static int lenBase[29 + 2] = { 3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
                               35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0 };
static char distBits[30 + 2] = { 0, 0, 0, 0, 1, 1, 2,  2,  3,  3,  4,  4,  5,  5,  6, 6,
                                 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 0, 0 };
static int distBase[30 + 2] = {
    1,   2,   3,   4,   5,   7,    9,    13,   17,   25,   33,   49,   65,    97,    129,
    193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

// Table to bit-reverse a byte.
static const unsigned char reverseTable[256] = {
#define R2(n) n, n + 128, n + 64, n + 192
#define R4(n) R2(n), R2(n + 32), R2(n + 16), R2(n + 48)
#define R6(n) R4(n), R4(n + 8), R4(n + 4), R4(n + 12)
    R6(0), R6(2), R6(1), R6(3)
};

static unsigned rev16(unsigned n) {
    return (reverseTable[n & 0xff] << 8) | reverseTable[(n >> 8) & 0xff];
}

static int bits(State* s, int n) {
    int v = s->bits & ((1 << n) - 1);
    s->bits >>= n;
    s->count -= n;
    while (s->count < 16) {
        CHECK(s->in != s->inend);
        s->bits |= (*s->in++) << s->count;
        s->count += 8;
    }
    return v;
}

static unsigned char* emit(State* s, int len) {
    s->out += len;
    CHECK(s->out <= s->outend);
    return s->out - len;
}

static void copy(State* s, const unsigned char* src, int len) {
    unsigned char* dest = emit(s, len);
    while (len--)
        *dest++ = *src++;
}

static int build(State* s, unsigned* tree, unsigned char* lens, unsigned int symcount) {
    unsigned int codes[16], first[16], counts[16] = { 0 };

    // Frequency count.
    for (unsigned int n = 0; n < symcount; n++)
        counts[lens[n]]++;

    // Distribute codes.
    counts[0] = codes[0] = first[0] = 0;
    for (unsigned int n = 1; n <= 15; n++) {
        codes[n] = (codes[n - 1] + counts[n - 1]) << 1;
        first[n] = first[n - 1] + counts[n - 1];
    }
    CHECK(first[15] + counts[15] <= symcount);

    // Insert keys into the tree for each symbol.
    for (unsigned int n = 0; n < symcount; n++) {
        int len = lens[n];
        if (len != 0) {
            unsigned code = codes[len]++, slot = first[len]++;
            tree[slot] = (code << (32 - len)) | (n << 4) | len;
        }
    }

    return first[15];
}

static int decode(State* s, unsigned tree[], int max) {
    // Find the next prefix code.
    unsigned lo = 0, hi = max, key;
    unsigned search = (rev16(s->bits) << 16) | 0xffff;
    while (lo < hi) {
        unsigned guess = (lo + hi) / 2;
        if (search < tree[guess])
            hi = guess;
        else
            lo = guess + 1;
    }

    // Pull out the key and check it.
    key = tree[lo - 1];
    CHECK(((search ^ key) >> (32 - (key & 0xf))) == 0);

    bits(s, key & 0xf);
    return (key >> 4) & 0xfff;
}

static void run(State* s, int sym) {
    int length = bits(s, lenBits[sym]) + lenBase[sym];
    int dsym = decode(s, s->distcodes, s->tdist);
    int offs = bits(s, distBits[dsym]) + distBase[dsym];
    copy(s, s->out - offs, length);
}

static void block(State* s) {
    for (;;) {
        int sym = decode(s, s->litcodes, s->tlit);
        if (sym < 256)
            *emit(s, 1) = (unsigned char)sym;
        else if (sym > 256)
            run(s, sym - 257);
        else
            break;
    }
}

static void stored(State* s) {
    // Uncompressed data block.
    int len;
    bits(s, s->count & 7);
    len = bits(s, 16);
    CHECK(((len ^ s->bits) & 0xffff) == 0xffff);
    CHECK(s->in + len <= s->inend);

    copy(s, s->in, len);
    s->in += len;
    bits(s, 16);
}

static void fixed(State* s) {
    // Fixed set of Huffman codes.
    int n;
    unsigned char lens[288 + 32];
    for (n = 0; n <= 143; n++)
        lens[n] = 8;
    for (n = 144; n <= 255; n++)
        lens[n] = 9;
    for (n = 256; n <= 279; n++)
        lens[n] = 7;
    for (n = 280; n <= 287; n++)
        lens[n] = 8;
    for (n = 0; n < 32; n++)
        lens[288 + n] = 5;

    // Build lit/dist trees.
    s->tlit = build(s, s->litcodes, lens, 288);
    s->tdist = build(s, s->distcodes, lens + 288, 32);
}

static void dynamic(State* s) {
    int n, i, nlit, ndist, nlen;
    unsigned char lenlens[19] = { 0 }, lens[288 + 32];
    nlit = 257 + bits(s, 5);
    ndist = 1 + bits(s, 5);
    nlen = 4 + bits(s, 4);
    for (n = 0; n < nlen; n++)
        lenlens[(int) order[n]] = (unsigned char)bits(s, 3);

    // Build the tree for decoding code lengths.
    s->tlen = build(s, s->lencodes, lenlens, 19);

    // Decode code lengths.
    for (n = 0; n < nlit + ndist;) {
        int sym = decode(s, s->lencodes, s->tlen);
        switch (sym) {
            case 16:
                for (i = 3 + bits(s, 2); i; i--, n++)
                    lens[n] = lens[n - 1];
                break;
            case 17:
                for (i = 3 + bits(s, 3); i; i--, n++)
                    lens[n] = 0;
                break;
            case 18:
                for (i = 11 + bits(s, 7); i; i--, n++)
                    lens[n] = 0;
                break;
            default:
                lens[n++] = (unsigned char)sym;
                break;
        }
    }

    // Build lit/dist trees.
    s->tlit = build(s, s->litcodes, lens, nlit);
    s->tdist = build(s, s->distcodes, lens + nlit, ndist);
}

int tigrInflate(void* out, unsigned outlen, const void* in, unsigned inlen) {
    int last;
    State* s = (State*)calloc(1, sizeof(State));

    // We assume we can buffer 2 extra bytes from off the end of 'in'.
    s->in = (unsigned char*)in;
    s->inend = s->in + inlen + 2;
    s->out = (unsigned char*)out;
    s->outend = s->out + outlen;
    s->bits = 0;
    s->count = 0;
    bits(s, 0);

    if (setjmp(s->jmp) == 1) {
        free(s);
        return 0;
    }

    do {
        last = bits(s, 1);
        switch (bits(s, 2)) {
            case 0:
                stored(s);
                break;
            case 1:
                fixed(s);
                block(s);
                break;
            case 2:
                dynamic(s);
                block(s);
                break;
            case 3:
                FAIL();
        }
    } while (!last);

    free(s);
    return 1;
}

#undef CHECK
#undef FAIL

//////// End of inlined file: tigr_inflate.c ////////

//////// Start of inlined file: tigr_print.c ////////

//#include "tigr_internal.h"
//////// Start of inlined file: tigr_font.h ////////

// Auto-generated by incbin.pl from font.png

const unsigned char tigr_font[] = {
	0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0xfd,0x00,0x00,0x00,0x5c,0x08,0x03,0x00,0x00,0x00,0x92,0xab,0x43,
	0x85,0x00,0x00,0x03,0x00,0x50,0x4c,0x54,0x45,0x5f,0x53,0x87,0x00,0x00,0x00,0x54,
	0x54,0x54,0xff,0xff,0xfa,0x31,0x2a,0x42,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,
	0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,
	0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,
	0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x4a,0x44,0xb7,0x5a,0x00,0x00,0x00,
	0x01,0x74,0x52,0x4e,0x53,0x00,0x40,0xe6,0xd8,0x66,0x00,0x00,0x00,0x01,0x62,0x4b,
	0x47,0x44,0x00,0x88,0x05,0x1d,0x48,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,
	0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,0x9a,0x9c,0x18,0x00,0x00,0x00,0x07,
	0x74,0x49,0x4d,0x45,0x07,0xe5,0x04,0x1c,0x16,0x0f,0x2b,0x56,0x9d,0x54,0x3b,0x00,
	0x00,0x0b,0x7c,0x49,0x44,0x41,0x54,0x78,0xda,0xed,0x5c,0x89,0x9a,0xec,0xac,0x0a,
	0x04,0xf5,0xfd,0x9f,0xf9,0x4e,0x94,0xa5,0x40,0x93,0x98,0xee,0x3e,0x73,0xe7,0xfb,
	0x67,0xce,0xd2,0x66,0x31,0x6a,0x29,0x51,0x4a,0x20,0xd4,0x7e,0xf3,0x1f,0x6a,0x34,
	0xfe,0xb4,0x5a,0xf9,0xf8,0x7f,0x24,0x54,0xf9,0xf8,0xf7,0x75,0xeb,0xeb,0x97,0xe0,
	0xd4,0x73,0x92,0x64,0xd7,0xab,0x90,0xa4,0xc3,0x91,0x59,0xb2,0x56,0x29,0x50,0xce,
	0xed,0xfa,0x71,0x74,0x94,0xd7,0x2f,0xd8,0xf5,0x7c,0x0e,0xf9,0xb1,0x74,0x69,0x15,
	0x13,0x14,0xfd,0x09,0xf4,0xd2,0x08,0x3e,0xfe,0xf6,0x53,0x86,0x9c,0xa5,0x8d,0xcc,
	0xd2,0x45,0x9a,0x2c,0xd0,0x7f,0x65,0x63,0x45,0xc1,0xa3,0x00,0xae,0x78,0x7d,0xb4,
	0xb9,0x9f,0xf3,0x57,0xb9,0x7a,0xfd,0x38,0xc3,0x7c,0x90,0x36,0xad,0xba,0x68,0x75,
	0x1c,0x8a,0x7e,0x05,0x3d,0x1f,0xcf,0xfa,0x18,0x0c,0xd8,0xad,0xff,0x96,0x9e,0xa3,
	0xd7,0xc0,0x36,0xec,0x90,0xe9,0x0c,0x7d,0x1d,0x68,0x6c,0x6c,0x7b,0x01,0x5f,0xa5,
	0x0d,0x74,0x23,0x25,0xe9,0xf5,0xd1,0xbb,0xbd,0x22,0x62,0xc1,0xa5,0xd7,0x53,0x2a,
	0x02,0x38,0x9a,0x35,0x04,0xa4,0x8f,0x90,0x16,0xf9,0x0a,0x7a,0xfa,0x7a,0xb6,0x8f,
	0xe9,0xd1,0x09,0x63,0x6c,0x8f,0x46,0x70,0xd1,0x7e,0x1d,0x08,0x07,0x20,0x41,0xdf,
	0x81,0x15,0x6b,0x03,0x11,0x1e,0xf6,0x3e,0xb1,0xde,0xec,0xc3,0x22,0x12,0x31,0x46,
	0x4d,0x9f,0xfd,0xaa,0x41,0x7b,0x5d,0xf3,0xb3,0xe7,0xe3,0x5a,0x3d,0x1d,0xbd,0xd6,
	0xff,0x1f,0xe0,0xa9,0x4b,0x48,0x2f,0x6c,0x0c,0xc2,0x78,0xe4,0x25,0xf4,0x32,0x6a,
	0x2a,0xf9,0xac,0xaf,0xeb,0xa8,0xa6,0x8e,0x54,0x87,0x27,0x8c,0xfd,0xd1,0x5f,0x63,
	0xb8,0xbc,0x39,0xe4,0xa3,0x63,0x12,0x2e,0x6f,0x49,0xbf,0xfe,0xd5,0x60,0xc6,0xf7,
	0x3d,0x4b,0xba,0xe6,0xb3,0x73,0xd6,0xeb,0xa3,0x11,0x2a,0xec,0x15,0x5f,0x83,0xf1,
	0xc8,0x53,0xf4,0xec,0xaf,0x50,0x92,0xfc,0xa3,0xdc,0x31,0x24,0x7a,0x8e,0xe8,0x0f,
	0xc1,0x18,0xd5,0xae,0x24,0x5f,0x85,0xa6,0x04,0xf4,0xde,0x8d,0x84,0xa8,0xb3,0xa4,
	0x6b,0x3e,0x3b,0xf7,0xfb,0x0d,0x85,0x7d,0x74,0xb6,0x74,0xe8,0xa8,0xe2,0x8d,0xb1,
	0x07,0xc9,0xaf,0xa3,0xdc,0xca,0x3a,0x24,0x32,0xa8,0x8e,0x9e,0x48,0x90,0x68,0x43,
	0x1b,0x4e,0xa3,0xa9,0xf5,0x36,0xeb,0x15,0x5a,0xa3,0xca,0x69,0xc8,0x37,0xbd,0xf7,
	0x2a,0xec,0x63,0xce,0x97,0x66,0xf4,0xf4,0x33,0x92,0xdf,0xfb,0x13,0x3b,0x43,0xc6,
	0x9e,0x6d,0xe2,0x8f,0x2b,0xde,0x84,0x5e,0x32,0xfb,0xdc,0x80,0x92,0x2e,0xd7,0x4d,
	0x06,0xf0,0xfd,0x96,0xc9,0x05,0x9f,0xb7,0x74,0xcc,0x4d,0xc7,0x73,0x30,0xe7,0x17,
	0xa2,0xd7,0xc7,0x1e,0x24,0x7f,0x4c,0x7e,0x36,0x9e,0x76,0x7a,0x48,0x98,0xbe,0xd0,
	0x32,0xf3,0xe4,0xf5,0x1e,0x06,0xa3,0xe7,0x28,0x92,0xb9,0x67,0x65,0x11,0x52,0x4d,
	0xf5,0xfa,0x91,0x3d,0x9c,0x5b,0xfe,0x42,0xf8,0x3c,0xe4,0x6f,0xd6,0xf1,0xa1,0x01,
	0x2f,0xa3,0xcf,0x05,0x9d,0x9d,0x7a,0xce,0x36,0x67,0x6a,0x51,0x1a,0xda,0x69,0x01,
	0xef,0xa6,0xcd,0x3a,0x3e,0xa3,0x7f,0x4d,0xf2,0x3f,0x83,0x3e,0x4a,0xc3,0x3f,0x44,
	0x9f,0x4b,0x4f,0x55,0xfd,0x7f,0xd0,0xaf,0x0e,0x7f,0x32,0xfa,0xdf,0xce,0x72,0x54,
	0x01,0xa5,0xac,0x6a,0xa5,0xf3,0x3c,0x65,0xfb,0x7d,0x5d,0x88,0xe0,0x7a,0x5a,0xc0,
	0x74,0xe1,0xca,0xeb,0xf9,0x65,0x85,0x34,0x55,0x3c,0xad,0x7c,0x34,0x1f,0x60,0x53,
	0xaa,0x2f,0x3c,0x35,0x90,0x92,0x21,0x20,0x63,0x3d,0x63,0xd5,0x66,0xce,0x48,0x05,
	0x67,0x12,0x82,0x64,0x44,0xab,0x64,0xd3,0x8c,0xd9,0x16,0xac,0x88,0xde,0xf5,0xfd,
	0xdb,0x8a,0x30,0xd5,0x82,0xf7,0xd1,0x83,0xd6,0x58,0x7c,0xd9,0xe5,0x26,0x7a,0x9b,
	0xa3,0xef,0x7a,0x98,0x6a,0x33,0x53,0xe1,0xbc,0x44,0x11,0xc9,0x48,0x56,0x66,0xc6,
	0xea,0x4d,0xc2,0x18,0x0b,0xa2,0x8f,0xc2,0xb3,0xaa,0x30,0x8d,0xa0,0xe8,0xcc,0xa6,
	0x25,0xad,0xd1,0x33,0x56,0x55,0x55,0xdd,0x57,0x32,0xc6,0xa6,0x60,0x1e,0x60,0x61,
	0x4d,0x6e,0x83,0xbf,0xaa,0x2e,0x07,0x8d,0x0b,0xad,0x76,0x71,0x52,0xc5,0x9a,0x43,
	0x2f,0x98,0xa6,0x9d,0xd0,0x9b,0x9a,0xae,0xe5,0xd8,0x98,0xdf,0xa0,0x77,0xed,0x86,
	0x61,0x11,0x3f,0x45,0x4f,0x2c,0x55,0xb9,0xfe,0xe4,0x10,0x1a,0x39,0xfa,0x63,0x35,
	0xf4,0x35,0x59,0xd0,0x93,0x6b,0xb2,0x31,0x4d,0xe8,0x59,0x15,0x57,0x4d,0xeb,0x44,
	0x4a,0xa6,0xb1,0x0f,0xe8,0x95,0xcb,0xc5,0x57,0x06,0xbb,0x3b,0x14,0xc8,0x71,0x57,
	0x64,0x81,0xde,0xe9,0x93,0xd0,0x4f,0xe1,0x4f,0xd0,0xf1,0x42,0x07,0x08,0x95,0x59,
	0x40,0xdf,0xe5,0xe6,0xec,0xb5,0x4a,0x95,0x26,0xc9,0x0f,0x20,0x16,0xe8,0xf9,0x72,
	0xd6,0x43,0xb4,0xda,0xd6,0x58,0x20,0xd7,0xc4,0x0f,0x16,0x73,0x9b,0x6f,0x10,0x60,
	0x3f,0x39,0xc3,0x10,0x9a,0xbd,0x46,0xcf,0x75,0x28,0xb0,0xb4,0xf1,0x1a,0xce,0xef,
	0xfd,0x0a,0x3d,0x8e,0xf9,0x2e,0x7a,0x7b,0x35,0x72,0xaf,0xac,0xd0,0x03,0x6f,0xb0,
	0x07,0x0a,0x9f,0xf1,0x28,0x25,0x43,0x4b,0xf4,0xc4,0x75,0xb5,0x72,0x05,0xb2,0x91,
	0x49,0x4a,0x24,0x1f,0x79,0x99,0x61,0x10,0xbb,0x3c,0xc9,0xc5,0x99,0xf9,0x84,0xcd,
	0xe4,0xfb,0x8a,0xc9,0xce,0xd9,0x2a,0x28,0x1c,0x27,0x00,0x1f,0x7b,0x5f,0xf1,0x74,
	0x7b,0x0d,0x09,0x58,0x3f,0x6b,0x43,0x33,0x76,0x52,0x61,0x24,0xa4,0x24,0x72,0x12,
	0x48,0x4a,0x24,0x1f,0xe1,0x39,0x21,0x2b,0x85,0xe6,0xeb,0xfd,0xaa,0x1e,0x20,0xab,
	0xc1,0x02,0x75,0xc7,0x2b,0xb2,0x20,0x16,0xba,0x15,0xcf,0x43,0x81,0xb2,0x17,0xc6,
	0xbe,0xdf,0x33,0xaa,0x2e,0x42,0x05,0x19,0x68,0x9c,0xdc,0x75,0x4d,0xf7,0xdf,0x28,
	0xa4,0xab,0xf4,0x3a,0x83,0xb7,0xf1,0x79,0x81,0xeb,0x1c,0x8d,0xce,0xd4,0xf2,0x9f,
	0x87,0xfe,0x9d,0x02,0xff,0xd0,0x3f,0x44,0xff,0xbb,0x59,0x8e,0x4f,0x85,0xbe,0x96,
	0x41,0x17,0xe1,0x6d,0x51,0x1f,0x73,0x6e,0x53,0x2d,0xe3,0xe2,0xf6,0xea,0x58,0x37,
	0x57,0x1b,0x9e,0x88,0x4e,0x0b,0x3b,0x9e,0x78,0xf7,0x53,0xe8,0x6b,0xc5,0x49,0xc9,
	0x16,0x2a,0x99,0x57,0x93,0xc2,0xf4,0xed,0xe8,0x75,0xb5,0x6c,0x1c,0x87,0xec,0x12,
	0x7d,0xdf,0xb5,0x5d,0x51,0x4b,0xa3,0xaa,0xf1,0x36,0x30,0x58,0xd3,0x13,0x60,0xec,
	0x51,0xbf,0x36,0xb5,0xe7,0x82,0xc1,0x42,0x41,0x14,0xaa,0xb9,0x64,0x74,0xb3,0xa2,
	0xdc,0xc8,0x1b,0xe3,0x64,0xf6,0x0e,0x7d,0x57,0x1b,0x16,0x06,0xb3,0x48,0x61,0x59,
	0xfb,0x74,0x71,0x39,0xa0,0x07,0xfd,0x1a,0x38,0x1d,0xef,0x33,0x5a,0xac,0x66,0x0f,
	0x3d,0xb0,0x59,0x35,0x49,0xd5,0x3a,0xec,0x6f,0x9d,0xd6,0xdd,0xec,0x6c,0x99,0x89,
	0x28,0xaa,0x9e,0xab,0x6d,0x8b,0x13,0xb2,0x01,0xe8,0x5d,0xbf,0x46,0x4e,0x77,0xab,
	0x42,0x33,0x58,0xce,0xee,0xf9,0x6c,0xa2,0xb3,0x66,0x53,0x85,0xb1,0x2f,0x9d,0xcc,
	0x1e,0xdb,0xca,0x77,0xe8,0x67,0xd2,0xc5,0x99,0xdc,0x5d,0x37,0x2b,0x8c,0x3d,0xa0,
	0x37,0x4e,0xb7,0x45,0x20,0xd4,0x72,0x76,0xc7,0xe8,0x92,0x66,0x4e,0xc6,0xeb,0xa2,
	0xe4,0x93,0x6c,0xcb,0x6f,0xa0,0xa7,0x6b,0xf4,0xb4,0x3b,0xf6,0x4e,0x70,0xae,0x59,
	0xcd,0x92,0xe5,0x98,0xa1,0xfb,0x84,0xd3,0x4c,0x2f,0x12,0x98,0x7c,0x56,0xe8,0xef,
	0xdf,0xfb,0x28,0xf9,0xc8,0x6f,0xdc,0x66,0xaa,0xec,0x02,0xc8,0xc6,0x29,0x7a,0x9b,
	0x21,0xae,0x58,0x4d,0xb4,0xc9,0x22,0xcb,0x11,0x12,0x83,0xb5,0xb2,0xed,0x5c,0xf0,
	0x4c,0x82,0x60,0xd6,0x43,0xdb,0x7e,0x79,0x80,0x9e,0xd0,0xc8,0x62,0xfc,0xc6,0x2e,
	0x83,0x2d,0xa5,0x2c,0xc8,0x88,0xac,0x78,0x89,0x9f,0x5c,0xb0,0x1a,0xe5,0xf3,0x58,
	0x90,0x36,0x43,0x48,0xcc,0x9a,0xe3,0x94,0xc0,0xaa,0xfc,0x71,0x36,0x86,0xd3,0x92,
	0x8d,0x69,0x0b,0xfd,0xf6,0x46,0x7e,0x36,0x3e,0x92,0x5b,0xb4,0xd3,0xa5,0x8b,0x05,
	0x1a,0xd5,0x8a,0xfc,0xd4,0x5d,0xed,0xa9,0x98,0x4c,0xda,0x5b,0xae,0xfd,0xe3,0xe8,
	0xd1,0xf8,0xe8,0xa6,0xf4,0x7c,0xe9,0xdf,0xa3,0x8f,0xed,0xf8,0x2e,0xf4,0x68,0x7c,
	0xb4,0x0a,0xa7,0x4b,0x17,0xe8,0x4d,0xd2,0x97,0x8c,0x76,0x1b,0x7d,0x6c,0xc7,0x6b,
	0xe8,0xff,0x3c,0xd6,0xa4,0x8f,0x80,0x2c,0xa4,0xfe,0x86,0x6d,0x65,0x77,0xc0,0x60,
	0xb3,0xf4,0x8f,0x53,0xd5,0xed,0x45,0x83,0x1d,0xc5,0xf8,0x8d,0x16,0x1d,0x5e,0x2a,
	0xa3,0x3a,0xbc,0x30,0x23,0xca,0x56,0xb4,0x39,0xbe,0x24,0x6b,0x04,0xb4,0x2d,0x0c,
	0xb5,0xfa,0x97,0x0d,0x17,0x84,0x2d,0x4b,0x16,0x03,0xfa,0xd9,0x27,0x61,0x89,0x5e,
	0xd4,0xc8,0xe1,0xc9,0xe4,0x9e,0x0e,0x3e,0x05,0x0e,0x99,0xf6,0x1b,0xcf,0xd1,0xbb,
	0xcf,0x58,0x51,0xa7,0x19,0xa8,0xe5,0x04,0xbd,0xe4,0x10,0x9f,0x83,0x5b,0xf4,0x5d,
	0xc9,0xe7,0x80,0x9e,0x16,0xe8,0x27,0xc7,0x8b,0x32,0x3c,0x28,0x64,0xf7,0xd4,0xfa,
	0xd0,0x37,0x75,0x15,0xb0,0x49,0x01,0xba,0x7c,0x70,0x05,0xcb,0x86,0xba,0x44,0x24,
	0xf4,0xa2,0xab,0x8e,0x71,0x3c,0x7a,0x20,0xd4,0x32,0x54,0xbe,0x68,0x64,0x9c,0x28,
	0xd2,0x3d,0xbf,0x17,0x52,0xc1,0xcd,0x50,0xae,0x24,0x5f,0x8d,0x69,0x66,0xad,0x13,
	0x2f,0x0f,0xdc,0xdc,0x1f,0xcc,0x4a,0x76,0xa6,0x87,0xa3,0xa3,0x71,0x90,0xfe,0x0f,
	0x5c,0x3e,0xc6,0xea,0xdd,0x74,0x11,0x07,0xb5,0x05,0xde,0x1b,0x82,0x71,0x24,0x25,
	0x58,0xc3,0x2e,0x29,0x9a,0x95,0x29,0x7e,0xc3,0x1a,0x09,0x06,0xbc,0x2d,0xf4,0x2c,
	0x96,0x19,0x05,0xc4,0x9b,0x92,0x4f,0xe6,0x8d,0x68,0x8f,0x89,0xe7,0x94,0x4a,0x7e,
	0x91,0x42,0xab,0x90,0x0d,0xeb,0xb2,0x93,0x55,0xb0,0x05,0xc7,0x96,0x65,0x23,0x74,
	0x04,0x5c,0xcd,0x77,0xff,0xb2,0xbe,0x71,0xcd,0xc9,0xc5,0x6f,0x6b,0xec,0x89,0x6e,
	0x25,0x3f,0x6f,0x95,0x1b,0x7a,0x99,0x66,0x44,0xc1,0x57,0xa7,0x4e,0x93,0x7c,0xf3,
	0xcd,0x6b,0x0b,0xa7,0x87,0xb5,0xdb,0xd0,0x7a,0xe1,0x45,0x1e,0xe4,0x46,0x57,0x33,
	0x28,0x2b,0x7a,0xa9,0x8e,0x77,0xfc,0xf6,0x64,0xec,0x75,0x7a,0xb6,0x4d,0x82,0x31,
	0x19,0xe9,0xdb,0x89,0x86,0x82,0x28,0xf9,0xd5,0x69,0xb4,0x28,0xf8,0x3c,0xbd,0xf7,
	0xe2,0x4a,0x76,0x8d,0x7e,0x39,0xe7,0xcf,0x93,0x8f,0xf2,0x20,0x53,0xf7,0x8d,0x6c,
	0x08,0x7a,0xab,0x6e,0xc7,0x7b,0x89,0xd4,0xfe,0xb9,0x8d,0x3e,0x4a,0xbe,0xd8,0x7f,
	0x0b,0x30,0x14,0xa5,0x1f,0x32,0xd7,0x85,0x59,0xef,0x5d,0xf4,0xea,0x16,0xeb,0xf6,
	0xda,0xba,0xf2,0x86,0xe3,0x7d,0xf4,0xa6,0xa6,0x35,0x73,0x9a,0x35,0xbd,0xbd,0x8c,
	0x49,0x09,0xcd,0x24,0x9e,0x9a,0x33,0x9b,0xb9,0xb3,0x0d,0x27,0x37,0x61,0x47,0xb2,
	0xe2,0xf9,0xd1,0x07,0xd0,0x4b,0x2d,0x60,0xd9,0x09,0xc4,0xcc,0x88,0xda,0xa6,0xef,
	0x56,0xd0,0x76,0xdc,0xac,0xd4,0x0c,0xf2,0xc6,0x56,0x79,0x6e,0x69,0xb8,0xda,0xf0,
	0xe8,0x7d,0xf4,0xab,0x5a,0xae,0x94,0xf4,0xe7,0xba,0xde,0x55,0xc5,0x7f,0xe8,0xff,
	0x4b,0xe8,0xff,0x58,0x0e,0x86,0x52,0x10,0xaa,0xfe,0x94,0xdd,0x14,0xec,0x9e,0xfa,
	0xae,0xc3,0x2e,0x66,0x5f,0x72,0x2c,0x52,0xa6,0xa1,0xfd,0x87,0x28,0x6e,0x0c,0xe3,
	0x46,0x49,0xf0,0xf7,0x26,0xa0,0x49,0xdc,0x12,0x7d,0x91,0x1f,0x64,0x56,0x44,0xee,
	0x42,0x94,0x8e,0x36,0x86,0x7f,0x8d,0x5e,0x75,0x70,0xb6,0xb8,0x1a,0x82,0x68,0x90,
	0xb1,0xe9,0x5e,0x86,0x66,0xc7,0xe2,0x23,0xaf,0xf1,0x01,0xc9,0x7b,0x18,0xb3,0xa8,
	0x5a,0x02,0x81,0x2c,0xac,0x81,0x0d,0x11,0x3d,0x46,0x02,0x50,0xe8,0x28,0xb6,0xf8,
	0x99,0x80,0x94,0x59,0xba,0x04,0x8e,0x1e,0xa1,0xaf,0x40,0x22,0x84,0xa1,0xcd,0xce,
	0x19,0x83,0xd5,0xa9,0xdb,0xd3,0xf0,0x76,0x63,0x73,0x8c,0x18,0xe0,0xc3,0xd4,0xd1,
	0x86,0xd2,0x8b,0xc8,0x44,0x3c,0x94,0x26,0xbb,0x76,0x5c,0x2e,0xd1,0xb3,0x4b,0x9c,
	0xc0,0x6f,0x03,0x68,0x91,0x72,0xa6,0xa3,0xfb,0x25,0xff,0x24,0x26,0xcb,0x76,0xb4,
	0x39,0x99,0x0f,0x1a,0x6b,0x70,0x8e,0x52,0xd0,0xae,0x28,0x8a,0xba,0x57,0xd4,0x96,
	0x61,0x7b,0xfb,0xcd,0x94,0x41,0x54,0xd5,0x2d,0x8c,0x47,0xb5,0xd2,0xb1,0xb3,0x99,
	0xb6,0xee,0xb9,0xc5,0xb7,0xc1,0xdf,0x37,0x89,0x9e,0xf2,0xdd,0x85,0xa3,0x1c,0x67,
	0xda,0xaa,0x67,0x3e,0x40,0x6f,0x5e,0x98,0xa5,0xd5,0x13,0x17,0x54,0x16,0x65,0xce,
	0xa9,0x2c,0x0f,0xd9,0x6b,0xb5,0xa2,0x44,0xca,0xbb,0x21,0x82,0xde,0xe3,0x52,0x18,
	0xf7,0xa3,0x0a,0x69,0x18,0x4f,0x0d,0x1b,0xd0,0x61,0xaf,0x3a,0xa3,0xd7,0x2a,0x8a,
	0xbf,0x61,0x43,0xd9,0x53,0x57,0x43,0xeb,0x30,0xeb,0xee,0x5b,0x85,0x67,0x1a,0x7b,
	0x67,0x52,0x34,0xb9,0xa0,0x5a,0x34,0x88,0xb1,0x61,0xf1,0x76,0x13,0xa7,0x18,0x95,
	0x48,0x88,0x12,0x9c,0xc7,0xde,0x26,0x43,0xe9,0x27,0xd8,0x80,0xf6,0x5d,0x5a,0xd6,
	0x50,0xa7,0x02,0xa3,0x33,0xc2,0xc3,0x5c,0xb8,0x47,0x2f,0x6b,0xfd,0x76,0x54,0x3c,
	0x9c,0xe8,0x2d,0xc9,0x9f,0x23,0x4a,0xfa,0x4c,0xc8,0xd5,0xd9,0x30,0xb3,0x9f,0xb9,
	0x44,0xba,0x43,0x64,0x1f,0x2d,0xce,0xdb,0xbd,0x1e,0xf6,0x11,0xbd,0xdd,0xd1,0xd1,
	0x34,0xbf,0xf7,0x90,0x0b,0xde,0xec,0xe1,0x66,0x7b,0x76,0xf4,0x0c,0x3d,0x27,0xb4,
	0xac,0x32,0xec,0x63,0xaf,0xee,0x68,0x46,0x38,0xc3,0x56,0xd3,0x88,0xe7,0x0b,0x30,
	0x31,0x0b,0x4c,0x86,0x81,0x26,0x3b,0x7a,0x70,0x51,0x3b,0x45,0x2f,0x9b,0x1d,0x22,
	0x40,0xbe,0x84,0xe4,0x23,0xa1,0x7b,0x0f,0xa2,0x11,0x6b,0x70,0x4d,0x2b,0x3a,0x8a,
	0xec,0x01,0x57,0x8c,0x24,0x27,0x9d,0xcd,0x81,0x02,0x31,0x8b,0x5f,0x86,0xfd,0x8b,
	0x38,0xf6,0x6e,0xbe,0x09,0xdb,0x5d,0x0b,0x35,0x51,0x05,0x88,0x2c,0x30,0x2c,0x1e,
	0xa9,0xc7,0xe2,0x83,0x98,0xac,0x2b,0xaf,0x89,0xc6,0x97,0xf6,0x8a,0x33,0xf4,0xab,
	0xc2,0xda,0x12,0x52,0xf2,0x1d,0x41,0x1d,0xe4,0x04,0x3d,0x11,0xc6,0x65,0xc5,0xa3,
	0x8d,0xcd,0xad,0x8c,0x7e,0x1e,0xa6,0xa5,0x2e,0xfe,0x8f,0xd0,0xa3,0x99,0xc3,0x03,
	0x1a,0xef,0xd0,0x9f,0x1c,0x6d,0x6c,0x6e,0x5d,0x6d,0x2d,0x7d,0x3f,0xfa,0x65,0x85,
	0xac,0x2f,0xf9,0x43,0xf4,0x6a,0xea,0xf9,0x63,0x39,0x57,0xd6,0x0c,0xb6,0x50,0xf9,
	0x90,0xd6,0xf4,0xed,0x81,0xe0,0x4a,0x45,0x18,0xeb,0x1f,0x1e,0x05,0x2d,0x16,0x62,
	0x03,0xfc,0x8e,0xea,0xba,0x35,0x7f,0x93,0x60,0xbb,0x21,0xd3,0x70,0x9f,0x3c,0xe8,
	0x0f,0x60,0x54,0x05,0xb6,0x7b,0xac,0xd2,0xbe,0x39,0x6d,0x31,0xc7,0x6e,0x36,0xd7,
	0xc0,0xcc,0x19,0xbd,0xbb,0x90,0xb3,0x07,0x6b,0x1b,0x6c,0xdd,0xf2,0x92,0x3e,0xd0,
	0x3b,0x4d,0x96,0x45,0x78,0x58,0xb7,0xcc,0x36,0x1b,0x32,0xa1,0x3f,0x79,0xd0,0x5b,
	0x8e,0x31,0xe4,0xe4,0x5e,0x5a,0x8b,0xcf,0x40,0xdc,0xa5,0x11,0x3d,0x31,0x68,0xed,
	0x67,0xc9,0xea,0x97,0x16,0x7e,0x31,0x5b,0x0d,0x10,0xe5,0x5a,0x62,0xc9,0xa2,0x27,
	0x4b,0x49,0x29,0xe1,0xb9,0x4a,0x9b,0x14,0xd1,0xd1,0x3f,0x72,0x27,0xb3,0x32,0xa2,
	0xcb,0x09,0x78,0xb8,0x4d,0x09,0x9b,0xe1,0x2a,0xfe,0x56,0x8f,0xcc,0x7a,0x18,0xa1,
	0x55,0xb9,0x82,0xec,0x94,0x2d,0xb7,0x20,0xed,0x6f,0xb5,0x3a,0x88,0xb5,0x72,0xfb,
	0xd9,0x14,0x30,0xa1,0x00,0xdd,0xfa,0x75,0x92,0xd0,0xc9,0x2f,0x87,0x2f,0x2e,0x3c,
	0x68,0x80,0x45,0x86,0x57,0xae,0x53,0x0c,0xc8,0xd2,0x9f,0x0f,0xdd,0x8e,0x14,0x3d,
	0xeb,0xd8,0xe7,0x30,0xb1,0x9d,0x34,0x47,0x65,0x99,0x3f,0xce,0x94,0xd0,0x55,0x1f,
	0xec,0x45,0x68,0xe5,0x21,0x64,0x73,0xec,0xdc,0x42,0x8f,0xa1,0x4a,0xb2,0x5f,0xff,
	0xb2,0xe4,0xeb,0xd8,0x97,0x29,0x5e,0x69,0x9d,0xec,0xa0,0xe7,0x47,0xc2,0x27,0x31,
	0x75,0x22,0x3b,0x1b,0xe8,0xe1,0x93,0x0f,0xd2,0x6e,0x44,0xff,0x54,0xf2,0x29,0x7f,
	0x3e,0xe0,0xc5,0x59,0x6f,0x1d,0x85,0x73,0x9b,0x8a,0x39,0x97,0x6b,0x0a,0xc2,0x3b,
	0x4b,0xd1,0x02,0xc9,0xb6,0xe1,0x34,0xde,0x7b,0xf0,0x3b,0x7b,0x90,0x9a,0x2b,0x9b,
	0xd3,0xa2,0x72,0x9e,0xac,0x7e,0xa7,0x70,0xa0,0xe9,0x1b,0x03,0x67,0xa9,0x19,0xf7,
	0x39,0x85,0xeb,0x9c,0x3e,0xe0,0xc1,0x3a,0xc5,0x9f,0x2d,0xd4,0x5e,0x0f,0x88,0x20,
	0xca,0x3a,0xc0,0x45,0xd2,0x76,0x55,0xe0,0x07,0x7e,0xec,0xf4,0x72,0xcb,0xfd,0xca,
	0x9b,0xe8,0x91,0xbd,0xfe,0x3e,0xf4,0x77,0xb0,0x7f,0x3a,0xfa,0x3f,0x96,0x73,0x49,
	0x2e,0x4c,0xdd,0x5e,0xb0,0x9c,0x4d,0x9e,0x21,0x9a,0x7e,0xa3,0xe0,0x6e,0x66,0x04,
	0xc9,0xa9,0xd1,0x37,0x70,0x1c,0x74,0x66,0xdb,0x20,0x17,0xca,0x56,0x2e,0xd1,0x5f,
	0xf3,0x0c,0xd9,0xed,0x77,0x2f,0x37,0x09,0x90,0x57,0xdd,0xa5,0x3c,0x22,0x5b,0xfc,
	0x16,0xc7,0x41,0x67,0xb6,0x17,0x58,0x8e,0x2b,0x7b,0xfb,0xcf,0x8c,0x65,0x5a,0xbc,
	0xdc,0x64,0xc7,0x4e,0xdc,0xa3,0x7a,0xdf,0x3e,0x21,0x59,0x66,0xa8,0x79,0x8d,0xe3,
	0xe0,0x8e,0x17,0xc4,0xe4,0x3c,0x4b,0x9f,0xea,0x87,0x1e,0x2a,0x86,0x01,0x49,0xf8,
	0x51,0x8f,0xdd,0xa2,0x2a,0x32,0x96,0xe7,0x1c,0x27,0xa2,0x0f,0x21,0x08,0xdb,0xa9,
	0x13,0x13,0x5e,0x7c,0x42,0x6a,0xf1,0x89,0x89,0x84,0xde,0xe9,0xd6,0x43,0x3d,0x97,
	0x3d,0x48,0x66,0x8f,0xe3,0x84,0xc0,0x92,0x09,0xfd,0xab,0x7a,0x7e,0xe4,0x78,0xf3,
	0xf7,0x48,0x52,0x3a,0xa3,0xa7,0x47,0x8d,0x07,0xa5,0x5d,0xbf,0x0f,0xc3,0x9f,0x41,
	0xff,0x82,0x9e,0xff,0xf8,0x99,0xcf,0xa1,0xb7,0x20,0x9c,0xf2,0x01,0xf4,0x2f,0x4b,
	0xfe,0x0e,0x21,0x01,0xe3,0x6f,0x44,0x3f,0x7d,0xf2,0x60,0xb7,0x28,0x77,0x60,0xde,
	0xe4,0x38,0x5c,0x13,0xab,0xd2,0x0b,0xe2,0xaf,0xb7,0x08,0xc2,0xb9,0x4d,0x8d,0x47,
	0xd4,0x1d,0x62,0x12,0x02,0x66,0xd0,0xc1,0xec,0x3b,0x38,0x0e,0x7b,0xa4,0x57,0xb8,
	0x90,0x7d,0x37,0xde,0x64,0x39,0x7b,0x1f,0xc6,0x6a,0xdf,0xac,0xe7,0xc6,0x70,0x3a,
	0x4a,0x2b,0xde,0xe7,0x58,0xce,0xcf,0x44,0x8f,0x7b,0x10,0x9c,0x6c,0x3c,0x9f,0x64,
	0x39,0x3f,0x13,0x3d,0x7e,0x3d,0xa4,0x24,0x1b,0xcf,0xef,0x66,0x39,0xff,0x03,0xca,
	0x1b,0xaa,0xdf,0x6a,0xd1,0xba,0xa4,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,
	0x42,0x60,0x82 };

int tigr_font_size = (int)sizeof(tigr_font);


//////// End of inlined file: tigr_font.h ////////

#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#endif

TigrFont tigrStockFont;
TigrFont* tfont = &tigrStockFont;

// Converts 8-bit codepage entries into Unicode code points.
static int cp1252[] = {
    0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021, 0x02c6, 0x2030, 0x0160, 0x2039, 0x0152,
    0xfffd, 0x017d, 0xfffd, 0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122,
    0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178, 0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6,
    0x00a7, 0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, 0x00b0, 0x00b1, 0x00b2, 0x00b3,
    0x00b4, 0x00b5, 0x00b6, 0x00b7, 0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, 0x00c0,
    0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd,
    0x00ce, 0x00cf, 0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, 0x00d8, 0x00d9, 0x00da,
    0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
    0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, 0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4,
    0x00f5, 0x00f6, 0x00f7, 0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};

static int border(Tigr* bmp, int x, int y) {
    TPixel top = tigrGet(bmp, 0, 0);
    TPixel c = tigrGet(bmp, x, y);
    return (c.r == top.r && c.g == top.g && c.b == top.b) || x >= bmp->w || y >= bmp->h;
}

static void scan(Tigr* bmp, int* x, int* y, int* rowh) {
    while (*y < bmp->h) {
        if (*x >= bmp->w) {
            *x = 0;
            (*y) += *rowh;
            *rowh = 1;
        }
        if (!border(bmp, *x, *y))
            return;
        (*x)++;
    }
}

/*
 * Watermarks are encoded vertically in the alpha channel using seven pixels
 * starting at x, y. The first and last alpha values contain the magic values
 * 0b10101010 and 0b01010101 respectively.
 */
static int readWatermark(Tigr* bmp, int x, int y, int* big, int* small) {
    const int magicHeader = 0xAA;
    const int magicFooter = 0x55;

    unsigned char watermark[7];

    for (int i = 0; i < 7; i++) {
        TPixel c = tigrGet(bmp, x, y + i);
        watermark[i] = c.a;
    }

    if (watermark[0] != magicHeader || watermark[6] != magicFooter) {
        return 0;
    }

    *big = watermark[1] | (watermark[2] << 8) | (watermark[3] << 16) | (watermark[4] << 24);
    *small = watermark[5];

    return 1;
}

int tigrLoadGlyphs(TigrFont* font, int codepage) {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    int rowh = 1;

    TigrGlyph* g;
    switch (codepage) {
        case TCP_ASCII:
            font->numGlyphs = 128 - 32;
            break;
        case TCP_1252:
            font->numGlyphs = 256 - 32;
            break;
        case TCP_UTF32:
            if (!readWatermark(font->bitmap, 0, 0, &font->numGlyphs, &rowh)) {
                return 0;
            }
            h = rowh;
            x = 1;
            break;
        default:
            errno = EINVAL;
            return 0;
    }

    font->glyphs = (TigrGlyph*)calloc(font->numGlyphs, sizeof(TigrGlyph));

    for (int index = 0; index < font->numGlyphs; index++) {
        // Look up the Unicode code point.
        g = &font->glyphs[index];

        if (codepage != TCP_UTF32) {
            // Find the next glyph.
            scan(font->bitmap, &x, &y, &rowh);

            if (y >= font->bitmap->h) {
                errno = EINVAL;
                return 0;
            }

            // Scan the width and height
            w = h = 0;
            while (!border(font->bitmap, x + w, y)) {
                w++;
            }

            while (!border(font->bitmap, x, y + h)) {
                h++;
            }
        }

        switch (codepage) {
            case TCP_ASCII:
                g->code = index + 32;
                break;
            case TCP_1252:
                if (index < 96) {
                    g->code = index + 32;
                } else {
                    g->code = cp1252[index - 96];
                }
                break;
            case TCP_UTF32:
                if (!readWatermark(font->bitmap, x, y, &g->code, &w)) {
                    // Maybe we are at the end of a row?
                    x = 0;
                    y += rowh;
                    if (!readWatermark(font->bitmap, x, y, &g->code, &w)) {
                        return 0;
                    }
                }
                x++;
                break;
            default:
                return 0;
        }

        g->x = x;
        g->y = y;
        g->w = w;
        g->h = h;
        x += w;
        if (h != font->glyphs[0].h) {
            errno = EINVAL;
            return 0;
        }

        if (h > rowh) {
            rowh = h;
        }
    }

    // Sort by code point.
    for (int i = 1; i < font->numGlyphs; i++) {
        int j = i;
        TigrGlyph g = font->glyphs[i];
        while (j > 0 && font->glyphs[j - 1].code > g.code) {
            font->glyphs[j] = font->glyphs[j - 1];
            j--;
        }
        font->glyphs[j] = g;
    }

    return 1;
}

TigrFont* tigrLoadFont(Tigr* bitmap, int codepage) {
    TigrFont* font = (TigrFont*)calloc(1, sizeof(TigrFont));
    font->bitmap = bitmap;
    if (!tigrLoadGlyphs(font, codepage)) {
        tigrFreeFont(font);
        return NULL;
    }
    return font;
}

void tigrFreeFont(TigrFont* font) {
    tigrFree(font->bitmap);
    free(font->glyphs);
    free(font);
}

static TigrGlyph* get(TigrFont* font, int code) {
    unsigned lo = 0, hi = font->numGlyphs;
    while (lo < hi) {
        unsigned guess = (lo + hi) / 2;
        if (code < font->glyphs[guess].code)
            hi = guess;
        else
            lo = guess + 1;
    }

    if (lo == 0 || font->glyphs[lo - 1].code != code)
        return &font->glyphs['?' - 32];
    else
        return &font->glyphs[lo - 1];
}

void tigrSetupFont(TigrFont* font) {
    // Load the stock font if needed.
    if (font == tfont && !tfont->bitmap) {
        tfont->bitmap = tigrLoadImageMem(tigr_font, tigr_font_size);
        tigrLoadGlyphs(tfont, 1252);
    }
}

void tigrPrint(Tigr* dest, TigrFont* font, int x, int y, TPixel color, const char* text, ...) {
    char tmp[1024];
    TigrGlyph* g;
    va_list args;
    const char* p;
    int start = x, c;

    tigrSetupFont(font);

    // Expand the formatting string.
    va_start(args, text);
    vsnprintf(tmp, sizeof(tmp), text, args);
    tmp[sizeof(tmp) - 1] = 0;
    va_end(args);

    // Print each glyph.
    p = tmp;
    while (*p) {
        p = tigrDecodeUTF8(p, &c);
        if (c == '\r')
            continue;
        if (c == '\n') {
            x = start;
            y += tigrTextHeight(font, "");
            continue;
        }
        g = get(font, c);
        tigrBlitTint(dest, font->bitmap, x, y, g->x, g->y, g->w, g->h, color);
        x += g->w;
    }
}

int tigrTextWidth(TigrFont* font, const char* text) {
    int x = 0, w = 0, c;
    tigrSetupFont(font);

    while (*text) {
        text = tigrDecodeUTF8(text, &c);
        if (c == '\n' || c == '\r') {
            x = 0;
        } else {
            x += get(font, c)->w;
            w = (x > w) ? x : w;
        }
    }
    return w;
}

int tigrTextHeight(TigrFont* font, const char* text) {
    int rowh, h, c;
    tigrSetupFont(font);

    h = rowh = get(font, 0)->h;
    while (*text) {
        text = tigrDecodeUTF8(text, &c);
        if (c == '\n' && *text)
            h += rowh;
    }
    return h;
}

//////// End of inlined file: tigr_print.c ////////

//////// Start of inlined file: tigr_win.c ////////

#ifndef TIGR_HEADLESS

//#include "tigr_internal.h"
#include <assert.h>

// not really windows stuff
TigrInternal* tigrInternal(Tigr* bmp) {
    assert(bmp->handle);
    return (TigrInternal*)(bmp + 1);
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#pragma comment(lib, "opengl32")  // glViewport
#pragma comment(lib, "shell32")   // CommandLineToArgvW
#pragma comment(lib, "user32")    // SetWindowLong
#pragma comment(lib, "gdi32")     // ChoosePixelFormat
#pragma comment(lib, "advapi32")  // RegSetValueEx

#define WIDGET_SCALE 3
#define WIDGET_FADE 16

int main(int argc, char* argv[]);

#ifndef TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
HKEY tigrRegKey;
#endif

#ifdef __TINYC__
#define CP_UTF8 65001
int WINAPI MultiByteToWideChar();
int WINAPI WideCharToMultiByte();
#endif

static wchar_t* unicode(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    wchar_t* dest = (wchar_t*)malloc(sizeof(wchar_t) * len);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, dest, len);
    return dest;
}

void tigrError(Tigr* bmp, const char* message, ...) {
    char tmp[1024];

    va_list args;
    va_start(args, message);
    _vsnprintf(tmp, sizeof(tmp), message, args);
    tmp[sizeof(tmp) - 1] = 0;
    va_end(args);

    MessageBoxW(bmp ? (HWND)bmp->handle : NULL, unicode(tmp), bmp ? tigrInternal(bmp)->wtitle : L"Error",
                MB_OK | MB_ICONERROR);
    exit(1);
}

void tigrEnterBorderlessWindowed(Tigr* bmp) {
    // Enter borderless windowed mode.
    MONITORINFO mi = { sizeof(mi) };
    TigrInternal* win = tigrInternal(bmp);

    GetWindowRect((HWND)bmp->handle, &win->oldPos);

    GetMonitorInfo(MonitorFromWindow((HWND)bmp->handle, MONITOR_DEFAULTTONEAREST), &mi);
    win->dwStyle = WS_VISIBLE | WS_POPUP;
    SetWindowLong((HWND)bmp->handle, GWL_STYLE, win->dwStyle);
    SetWindowPos((HWND)bmp->handle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, 0);
}

void tigrLeaveBorderlessWindowed(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);

    win->dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
    SetWindowLong((HWND)bmp->handle, GWL_STYLE, win->dwStyle);

    SetWindowPos((HWND)bmp->handle, NULL, win->oldPos.left, win->oldPos.top, win->oldPos.right - win->oldPos.left,
                 win->oldPos.bottom - win->oldPos.top, 0);
}

void tigrWinUpdateWidgets(Tigr* bmp, int dw, int dh) {
    POINT pt;
    int i, x, clicked = 0;
    char str[8];
    TPixel col;
    TPixel off = tigrRGB(255, 255, 255);
    TPixel on = tigrRGB(0, 200, 255);
    TigrInternal* win = tigrInternal(bmp);
    (void)dh;

    tigrClear(win->widgets, tigrRGBA(0, 0, 0, 0));

    if (!(win->dwStyle & WS_POPUP)) {
        win->widgetsWanted = 0;
        win->widgetAlpha = 0;
        return;
    }

    // See if we want to be showing widgets or not.
    GetCursorPos(&pt);
    ScreenToClient((HWND)bmp->handle, &pt);
    if (pt.y == 0)
        win->widgetsWanted = 1;
    if (pt.y > win->widgets->h * WIDGET_SCALE)
        win->widgetsWanted = 0;

    // Track the alpha.
    if (win->widgetsWanted)
        win->widgetAlpha = (win->widgetAlpha <= 255 - WIDGET_FADE) ? win->widgetAlpha + WIDGET_FADE : 255;
    else
        win->widgetAlpha = (win->widgetAlpha >= WIDGET_FADE) ? win->widgetAlpha - WIDGET_FADE : 0;

    // Get relative coords.
    pt.x -= (dw - win->widgets->w * WIDGET_SCALE);
    pt.x /= WIDGET_SCALE;
    pt.y /= WIDGET_SCALE;

    tigrClear(win->widgets, tigrRGBA(0, 0, 0, win->widgetAlpha));

    // Render it.
    for (i = 0; i < 3; i++) {
        switch (i) {
            case 0:
                str[0] = '_';
                str[1] = 0;
                break;  // "_" (minimize)
            case 1:
                str[0] = 0xEF;
                str[1] = 0xBF;
                str[2] = 0xBD;
                str[3] = 0;
                break;  // "[]" (maximize)
            case 2:
                str[0] = 0xC3;
                str[1] = 0x97;
                str[2] = 0;
                break;  // "x" (close)
        }
        x = win->widgets->w + (i - 3) * 12;
        if (i == 2)
            off = tigrRGB(255, 0, 0);
        if (pt.x >= x && pt.x < x + 10 && pt.y < win->widgets->h) {
            col = on;
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
                clicked |= 1 << i;
        } else {
            col = off;
        }
        col.a = win->widgetAlpha;
        tigrPrint(win->widgets, tfont, x, 2, col, str);
    }

    if (clicked & 1)
        ShowWindow((HWND)bmp->handle, SW_MINIMIZE);
    if (clicked & 2)
        tigrLeaveBorderlessWindowed(bmp);
    if (clicked & 4)
        SendMessage((HWND)bmp->handle, WM_CLOSE, 0, 0);
}

void tigrUpdate(Tigr* bmp) {
    MSG msg;
    RECT rc;
    int dw, dh;
    TigrInternal* win = tigrInternal(bmp);

    if (!win->shown) {
        win->shown = 1;
        UpdateWindow((HWND)bmp->handle);
        ShowWindow((HWND)bmp->handle, SW_SHOW);
    }

    // Get the window size.
    GetClientRect((HWND)bmp->handle, &rc);
    dw = rc.right - rc.left;
    dh = rc.bottom - rc.top;

    // Update the widget overlay.
    tigrWinUpdateWidgets(bmp, dw, dh);

    if (!tigrGAPIBegin(bmp)) {
        tigrGAPIPresent(bmp, dw, dh);
        SwapBuffers(win->gl.dc);
        tigrGAPIEnd(bmp);
    }

    memcpy(win->prev, win->keys, 256);

    // Run the message pump.
    while (PeekMessage(&msg, (HWND)bmp->handle, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALFARPROC_)(int);
static PFNWGLSWAPINTERVALFARPROC_ wglSwapIntervalEXT_ = 0;

int tigrGAPIBegin(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);

    return wglMakeCurrent(win->gl.dc, win->gl.hglrc) ? 0 : -1;
}

int tigrGAPIEnd(Tigr* bmp) {
    (void)bmp;
    return wglMakeCurrent(NULL, NULL) ? 0 : -1;
}

static BOOL UnadjustWindowRectEx(LPRECT prc, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle) {
    BOOL fRc;
    RECT rc;
    SetRectEmpty(&rc);
    fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
    if (fRc) {
        prc->left -= rc.left;
        prc->top -= rc.top;
        prc->right -= rc.right;
        prc->bottom -= rc.bottom;
    }
    return fRc;
}

LRESULT CALLBACK tigrWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Tigr* bmp;
    TigrInternal* win = NULL;
    RECT rc;
    int dw, dh;

    GetClientRect(hWnd, &rc);
    dw = rc.right - rc.left;
    dh = rc.bottom - rc.top;

    bmp = (Tigr*)GetPropW(hWnd, L"Tigr");
    if (bmp)
        win = tigrInternal(bmp);

    switch (message) {
        case WM_PAINT:
            if (!tigrGAPIBegin(bmp)) {
                tigrGAPIPresent(bmp, dw, dh);
                SwapBuffers(win->gl.dc);
                tigrGAPIEnd(bmp);
            }
            ValidateRect(hWnd, NULL);
            break;
        case WM_CLOSE:
            if (win)
                win->closed = 1;
            break;
        case WM_GETMINMAXINFO:
            if (bmp) {
                MINMAXINFO* info = (MINMAXINFO*)lParam;
                RECT rc;
                rc.left = 0;
                rc.top = 0;
                if (win->flags & TIGR_AUTO) {
                    rc.right = 32;
                    rc.bottom = 32;
                } else {
                    int minscale = tigrEnforceScale(1, win->flags);
                    rc.right = bmp->w * minscale;
                    rc.bottom = bmp->h * minscale;
                }
                AdjustWindowRectEx(&rc, win->dwStyle, FALSE, 0);
                info->ptMinTrackSize.x = rc.right - rc.left;
                info->ptMinTrackSize.y = rc.bottom - rc.top;
            }
            return 0;
        case WM_SIZING:
            if (win) {
                // Calculate scale-constrained sizes.
                RECT* rc = (RECT*)lParam;
                int dx, dy;
                UnadjustWindowRectEx(rc, win->dwStyle, FALSE, 0);
                dx = (rc->right - rc->left) % win->scale;
                dy = (rc->bottom - rc->top) % win->scale;
                switch (wParam) {
                    case WMSZ_LEFT:
                        rc->left += dx;
                        break;
                    case WMSZ_RIGHT:
                        rc->right -= dx;
                        break;
                    case WMSZ_TOP:
                        rc->top += dy;
                        break;
                    case WMSZ_TOPLEFT:
                        rc->left += dx;
                        rc->top += dy;
                        break;
                    case WMSZ_TOPRIGHT:
                        rc->right -= dx;
                        rc->top += dy;
                        break;
                    case WMSZ_BOTTOM:
                        rc->bottom -= dy;
                        break;
                    case WMSZ_BOTTOMLEFT:
                        rc->left += dx;
                        rc->bottom -= dy;
                        break;
                    case WMSZ_BOTTOMRIGHT:
                        rc->right -= dx;
                        rc->bottom -= dy;
                        break;
                }
                AdjustWindowRectEx(rc, win->dwStyle, FALSE, 0);
            }
            return TRUE;
        case WM_SIZE:
            if (win) {
                if (wParam != SIZE_MINIMIZED) {
                    // Detect window size changes and update our bitmap accordingly.
                    dw = LOWORD(lParam);
                    dh = HIWORD(lParam);
                    if (win->flags & TIGR_AUTO) {
                        tigrResize(bmp, dw / win->scale, dh / win->scale);
                    } else {
                        win->scale = tigrEnforceScale(tigrCalcScale(bmp->w, bmp->h, dw, dh), win->flags);
                    }
                    tigrPosition(bmp, win->scale, dw, dh, win->pos);
                }

                // If someone tried to maximize us (e.g. via shortcut launch options),
                // prefer instead to be borderless.
                if (wParam == SIZE_MAXIMIZED) {
                    ShowWindow((HWND)bmp->handle, SW_NORMAL);
                    tigrEnterBorderlessWindowed(bmp);
                }
            }
            return 0;
#ifndef TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
        case WM_WINDOWPOSCHANGED: {
            // Save our position.
            WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
            GetWindowPlacement(hWnd, &wp);
            if (win->dwStyle & WS_POPUP)
                wp.showCmd = SW_MAXIMIZE;
            RegSetValueExW(tigrRegKey, win->wtitle, 0, REG_BINARY, (BYTE*)&wp, sizeof(wp));
            return DefWindowProcW(hWnd, message, wParam, lParam);
        }
#endif
        case WM_ACTIVATE:
            if (win) {
                memset(win->keys, 0, 256);
                memset(win->prev, 0, 256);
                win->lastChar = 0;
            }
            return 0;
        case WM_CHAR:
            if (win) {
                if (wParam == '\r') {
                    wParam = '\n';
                }
                int repeating = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;
                if (!repeating) {
                    win->lastChar = wParam;
                }
            }
            return DefWindowProcW(hWnd, message, wParam, lParam);
        case WM_MENUCHAR:
            // Disable beep on Alt+Enter
            if (LOWORD(wParam) == VK_RETURN)
                return MNC_CLOSE << 16;
            return DefWindowProcW(hWnd, message, wParam, lParam);
        case WM_SYSKEYDOWN:
            if (win) {
                if (wParam == VK_RETURN) {
                    // Alt+Enter
                    if (win->dwStyle & WS_POPUP)
                        tigrLeaveBorderlessWindowed(bmp);
                    else
                        tigrEnterBorderlessWindowed(bmp);
                    return 0;
                }
            }
            // fall-thru
        case WM_KEYDOWN:
            if (win)
                win->keys[wParam] = 1;
            return DefWindowProcW(hWnd, message, wParam, lParam);
        case WM_SYSKEYUP:
            // fall-thru
        case WM_KEYUP:
            if (win)
                win->keys[wParam] = 0;
            return DefWindowProcW(hWnd, message, wParam, lParam);
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

Tigr* tigrWindow(int w, int h, const char* title, int flags) {
    WNDCLASSEXW wcex = { 0 };
    int maxW, maxH, scale;
    HWND hWnd;
    DWORD dwStyle;
    RECT rc;
    DWORD err;
    Tigr* bmp;
    TigrInternal* win;
#ifndef TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
    WINDOWPLACEMENT wp;
    DWORD wpsize = sizeof(wp);
#endif

    wchar_t* wtitle = unicode(title);

// Find our registry key.
#ifndef TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
    RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\TIGR", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                    &tigrRegKey, NULL);
#endif

    // Register a window class.
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = tigrWndProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = L"TIGR";
    RegisterClassExW(&wcex);

    if (flags & TIGR_AUTO) {
        // Always use a 1:1 pixel size.
        scale = 1;
    } else {
        // See how big we can make it and still fit on-screen.
        maxW = GetSystemMetrics(SM_CXSCREEN) * 3 / 4;
        maxH = GetSystemMetrics(SM_CYSCREEN) * 3 / 4;
        scale = tigrCalcScale(w, h, maxW, maxH);
    }

    scale = tigrEnforceScale(scale, flags);

    // Get the final window size.
    dwStyle = WS_OVERLAPPEDWINDOW;
    rc.left = 0;
    rc.top = 0;
    rc.right = w * scale;
    rc.bottom = h * scale;
    AdjustWindowRect(&rc, dwStyle, FALSE);

    // Make a window.
    hWnd = CreateWindowW(L"TIGR", wtitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                         NULL, NULL, wcex.hInstance, NULL);
    err = GetLastError();
    if (!hWnd)
        ExitProcess(1);

    if (flags & TIGR_NOCURSOR) {
        ShowCursor(FALSE);
    }

    // Wrap a bitmap around it.
    bmp = tigrBitmap2(w, h, sizeof(TigrInternal));
    bmp->handle = hWnd;

    // Set up the Windows parts.
    win = tigrInternal(bmp);
    win->dwStyle = dwStyle;
    win->wtitle = wtitle;
    win->shown = 0;
    win->closed = 0;
    win->scale = scale;
    win->lastChar = 0;
    win->flags = flags;

    win->p1 = win->p2 = win->p3 = 0;
    win->p4 = 1;

    win->widgetsWanted = 0;
    win->widgetAlpha = 0;
    win->widgetsScale = WIDGET_SCALE;
    win->widgets = tigrBitmap(40, 14);

    SetPropW(hWnd, L"Tigr", bmp);

    tigrGAPICreate(bmp);

    if (flags & TIGR_FULLSCREEN) {
        tigrEnterBorderlessWindowed(bmp);
    } else {
// Try and restore our window position.
#ifndef TIGR_DO_NOT_PRESERVE_WINDOW_POSITION
        if (RegQueryValueExW(tigrRegKey, wtitle, NULL, NULL, (BYTE*)&wp, &wpsize) == ERROR_SUCCESS) {
            if (wp.showCmd == SW_MAXIMIZE)
                tigrEnterBorderlessWindowed(bmp);
            else
                SetWindowPlacement(hWnd, &wp);
        }
#endif
    }

    wglSwapIntervalEXT_ = (PFNWGLSWAPINTERVALFARPROC_)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT_)
        wglSwapIntervalEXT_(1);

    return bmp;
}

void tigrFree(Tigr* bmp) {
    if (bmp->handle) {
        TigrInternal* win = tigrInternal(bmp);
        tigrGAPIDestroy(bmp);

        if (win->gl.hglrc && !wglDeleteContext(win->gl.hglrc)) {
            tigrError(bmp, "Cannot delete OpenGL context.\n");
        }
        win->gl.hglrc = NULL;

        if (win->gl.dc && !ReleaseDC((HWND)bmp->handle, win->gl.dc)) {
            tigrError(bmp, "Cannot release OpenGL device context.\n");
        }
        win->gl.dc = NULL;

        DestroyWindow((HWND)bmp->handle);
        free(win->wtitle);
        tigrFree(win->widgets);
    }
    free(bmp->pix);
    free(bmp);
}

int tigrClosed(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    int val = win->closed;
    win->closed = 0;
    return val;
}

float tigrTime() {
    static int first = 1;
    static LARGE_INTEGER prev;

    LARGE_INTEGER cnt, freq;
    ULONGLONG diff;
    QueryPerformanceCounter(&cnt);
    QueryPerformanceFrequency(&freq);

    if (first) {
        first = 0;
        prev = cnt;
    }

    diff = cnt.QuadPart - prev.QuadPart;
    prev = cnt;
    return (float)(diff / (double)freq.QuadPart);
}

void tigrMouse(Tigr* bmp, int* x, int* y, int* buttons) {
    POINT pt;
    TigrInternal* win;

    win = tigrInternal(bmp);
    GetCursorPos(&pt);
    ScreenToClient((HWND)bmp->handle, &pt);
    *x = (pt.x - win->pos[0]) / win->scale;
    *y = (pt.y - win->pos[1]) / win->scale;
    *buttons = 0;
    if (GetFocus() != bmp->handle)
        return;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        *buttons |= 1;
    if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
        *buttons |= 2;
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        *buttons |= 4;
}

int tigrTouch(Tigr* bmp, TigrTouchPoint* points, int maxPoints) {
    int buttons = 0;
    if (maxPoints > 0) {
        tigrMouse(bmp, &points[0].x, &points[1].y, &buttons);
    }
    return buttons ? 1 : 0;
}

static int tigrWinVK(int key) {
    if (key >= 'A' && key <= 'Z')
        return key;
    if (key >= '0' && key <= '9')
        return key;
    switch (key) {
        case TK_BACKSPACE:
            return VK_BACK;
        case TK_TAB:
            return VK_TAB;
        case TK_RETURN:
            return VK_RETURN;
        case TK_SHIFT:
            return VK_SHIFT;
        case TK_CONTROL:
            return VK_CONTROL;
        case TK_ALT:
            return VK_MENU;
        case TK_PAUSE:
            return VK_PAUSE;
        case TK_CAPSLOCK:
            return VK_CAPITAL;
        case TK_ESCAPE:
            return VK_ESCAPE;
        case TK_SPACE:
            return VK_SPACE;
        case TK_PAGEUP:
            return VK_PRIOR;
        case TK_PAGEDN:
            return VK_NEXT;
        case TK_END:
            return VK_END;
        case TK_HOME:
            return VK_HOME;
        case TK_LEFT:
            return VK_LEFT;
        case TK_UP:
            return VK_UP;
        case TK_RIGHT:
            return VK_RIGHT;
        case TK_DOWN:
            return VK_DOWN;
        case TK_INSERT:
            return VK_INSERT;
        case TK_DELETE:
            return VK_DELETE;
        case TK_LWIN:
            return VK_LWIN;
        case TK_RWIN:
            return VK_RWIN;
        // case TK_APPS: return VK_APPS; // this key doesn't exist on OS X
        case TK_PAD0:
            return VK_NUMPAD0;
        case TK_PAD1:
            return VK_NUMPAD1;
        case TK_PAD2:
            return VK_NUMPAD2;
        case TK_PAD3:
            return VK_NUMPAD3;
        case TK_PAD4:
            return VK_NUMPAD4;
        case TK_PAD5:
            return VK_NUMPAD5;
        case TK_PAD6:
            return VK_NUMPAD6;
        case TK_PAD7:
            return VK_NUMPAD7;
        case TK_PAD8:
            return VK_NUMPAD8;
        case TK_PAD9:
            return VK_NUMPAD9;
        case TK_PADMUL:
            return VK_MULTIPLY;
        case TK_PADADD:
            return VK_ADD;
        case TK_PADENTER:
            return VK_SEPARATOR;
        case TK_PADSUB:
            return VK_SUBTRACT;
        case TK_PADDOT:
            return VK_DECIMAL;
        case TK_PADDIV:
            return VK_DIVIDE;
        case TK_F1:
            return VK_F1;
        case TK_F2:
            return VK_F2;
        case TK_F3:
            return VK_F3;
        case TK_F4:
            return VK_F4;
        case TK_F5:
            return VK_F5;
        case TK_F6:
            return VK_F6;
        case TK_F7:
            return VK_F7;
        case TK_F8:
            return VK_F8;
        case TK_F9:
            return VK_F9;
        case TK_F10:
            return VK_F10;
        case TK_F11:
            return VK_F11;
        case TK_F12:
            return VK_F12;
        case TK_NUMLOCK:
            return VK_NUMLOCK;
        case TK_SCROLL:
            return VK_SCROLL;
        case TK_LSHIFT:
            return VK_LSHIFT;
        case TK_RSHIFT:
            return VK_RSHIFT;
        case TK_LCONTROL:
            return VK_LCONTROL;
        case TK_RCONTROL:
            return VK_RCONTROL;
        case TK_LALT:
            return VK_LMENU;
        case TK_RALT:
            return VK_RMENU;
        case TK_SEMICOLON:
            return VK_OEM_1;
        case TK_EQUALS:
            return VK_OEM_PLUS;
        case TK_COMMA:
            return VK_OEM_COMMA;
        case TK_MINUS:
            return VK_OEM_MINUS;
        case TK_DOT:
            return VK_OEM_PERIOD;
        case TK_SLASH:
            return VK_OEM_2;
        case TK_BACKTICK:
            return VK_OEM_3;
        case TK_LSQUARE:
            return VK_OEM_4;
        case TK_BACKSLASH:
            return VK_OEM_5;
        case TK_RSQUARE:
            return VK_OEM_6;
        case TK_TICK:
            return VK_OEM_7;
    }
    return 0;
}

int tigrKeyDown(Tigr* bmp, int key) {
    TigrInternal* win;
    int k = tigrWinVK(key);
    if (GetFocus() != bmp->handle)
        return 0;
    win = tigrInternal(bmp);
    return win->keys[k] && !win->prev[k];
}

int tigrKeyHeld(Tigr* bmp, int key) {
    TigrInternal* win;
    int k = tigrWinVK(key);
    if (GetFocus() != bmp->handle)
        return 0;
    win = tigrInternal(bmp);
    return win->keys[k];
}

int tigrReadChar(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    int c = win->lastChar;
    win->lastChar = 0;
    return c;
}

// We supply our own WinMain and just chain through to the user's
// real entry point.
#ifdef UNICODE
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    int n, argc;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    char** argv = (char**)calloc(argc + 1, sizeof(int));

    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    for (n = 0; n < argc; n++) {
        int len = WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, 0, 0, NULL, NULL);
        argv[n] = (char*)malloc(len);
        WideCharToMultiByte(CP_UTF8, 0, wargv[n], -1, argv[n], len, NULL, NULL);
    }
    return main(argc, argv);
}
#endif

#endif // #ifndef TIGR_HEADLESS
//////// End of inlined file: tigr_win.c ////////

//////// Start of inlined file: tigr_osx.c ////////

#ifndef TIGR_HEADLESS

// originally based on https://github.com/jimon/osx_app_in_plain_c

//#include "tigr_internal.h"
//////// Start of inlined file: tigr_objc.h ////////

#ifndef TIGR_OBJC_H
#define TIGR_OBJC_H

#if defined(__IOS__) || defined(__MACOS__)

#if defined(__OBJC__) && __has_feature(objc_arc)
#error "Can't compile as objective-c code!"
#endif

// ABI is a bit different between platforms
#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#endif
#ifdef __i386__
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#else
#define abi_objc_msgSend_fpret objc_msgSend
#endif

#define objc_msgSendSuper_t(RET, ...) ((RET(*)(struct objc_super*, SEL, ##__VA_ARGS__))objc_msgSendSuper)
#define objc_msgSend_t(RET, ...) ((RET(*)(id, SEL, ##__VA_ARGS__))objc_msgSend)
#define objc_msgSend_stret_t(RET, ...) ((RET(*)(id, SEL, ##__VA_ARGS__))abi_objc_msgSend_stret)
#define objc_msgSend_id objc_msgSend_t(id)
#define objc_msgSend_void objc_msgSend_t(void)
#define objc_msgSend_void_id objc_msgSend_t(void, id)
#define objc_msgSend_void_bool objc_msgSend_t(void, bool)

#define sel(NAME) sel_registerName(NAME)
#define class(NAME) ((id)objc_getClass(NAME))
#define makeClass(NAME, SUPER) objc_allocateClassPair((Class)objc_getClass(SUPER), NAME, 0)

// Check here to get the signature right: https://nshipster.com/type-encodings/
#define addMethod(CLASS, NAME, IMPL, SIGNATURE)                       \
    if (!class_addMethod(CLASS, sel(NAME), (IMP)(IMPL), (SIGNATURE))) \
    assert(false)

#define addIvar(CLASS, NAME, SIZE, SIGNATURE)                           \
    if (!class_addIvar(CLASS, NAME, SIZE, rint(log2(SIZE)), SIGNATURE)) \
    assert(false)

#define objc_alloc(CLASS) objc_msgSend_id(class(CLASS), sel("alloc"))

#if __LP64__ || NS_BUILD_32_LIKE_64
#define NSIntegerEncoding "q"
#define NSUIntegerEncoding "L"
#else
#define NSIntegerEncoding "i"
#define NSUIntegerEncoding "I"
#endif

#endif  // defined(__IOS__) || defined (__MACOS__)
#endif  // TIGR_OBJC_H

//////// End of inlined file: tigr_objc.h ////////


#if __MACOS__

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <mach/mach_time.h>
#include <objc/NSObjCRuntime.h>
#include <objc/message.h>
#include <objc/objc.h>
#include <objc/runtime.h>

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#else
// this is how they are defined originally
#include <CoreGraphics/CGBase.h>
#include <CoreGraphics/CGGeometry.h>
typedef CGPoint NSPoint;
typedef CGSize NSSize;
typedef CGRect NSRect;

enum {
    NSKeyDown = 10,
    NSKeyDownMask = 1 << NSKeyDown,
    NSKeyUp = 11,
    NSKeyUpMask = 1 << NSKeyUp,
};

NSUInteger NSAllEventMask = NSUIntegerMax;

extern id NSApp;
extern id const NSDefaultRunLoopMode;

#define NSApplicationActivationPolicyRegular 0
#endif

bool terminated = false;

static uint64_t tigrTimestamp = 0;

void _tigrResetTime(void) {
    tigrTimestamp = mach_absolute_time();
}

TigrInternal* _tigrInternalCocoa(id window) {
    if (!window)
        return NULL;

    id wdg = objc_msgSend_id(window, sel("delegate"));
    if (!wdg)
        return NULL;

    Tigr* bmp = 0;
    object_getInstanceVariable(wdg, "tigrHandle", (void**)&bmp);
    return bmp ? tigrInternal(bmp) : NULL;
}

// we gonna construct objective-c class by hand in runtime, so wow, so hacker!
NSUInteger applicationShouldTerminate(id self, SEL sel, id sender) {
    terminated = true;
    return 0;
}

void windowWillClose(id self, SEL _sel, id notification) {
    NSUInteger value = true;
    object_setInstanceVariable(self, "closed", (void*)value);
    object_setInstanceVariable(self, "tigrHandle", (void*)0);
}

void windowDidEnterFullScreen(id self, SEL _sel, id notification) {
    NSUInteger value = true;
    object_setInstanceVariable(self, "visible", (void*)value);
}

void windowDidResize(id self, SEL _sel, id notification) {
    TigrInternal* win;
    Tigr* bmp = 0;
    object_getInstanceVariable(self, "tigrHandle", (void**)&bmp);
    win = bmp ? tigrInternal(bmp) : NULL;
    if (win) {
        win->mouseButtons = 0;
    }
}

void windowDidBecomeKey(id self, SEL _sel, id notification) {
    TigrInternal* win;
    Tigr* bmp = 0;
    object_getInstanceVariable(self, "tigrHandle", (void**)&bmp);
    win = bmp ? tigrInternal(bmp) : NULL;

    if (win) {
        memset(win->keys, 0, 256);
        memset(win->prev, 0, 256);
        win->lastChar = 0;
        win->mouseButtons = 0;
    }
}

void mouseEntered(id self, SEL _sel, id event) {
    id window = objc_msgSend_id(event, sel("window"));
    TigrInternal* win = _tigrInternalCocoa(window);
    if (win) {
        win->mouseInView = 1;
        if (win->flags & TIGR_NOCURSOR) {
            objc_msgSend_id(class("NSCursor"), sel("hide"));
        }
    }
}

void mouseExited(id self, SEL _sel, id event) {
    id window = objc_msgSend_id(event, sel("window"));
    TigrInternal* win = _tigrInternalCocoa(window);
    if (win) {
        win->mouseInView = 0;
        if (win->flags & TIGR_NOCURSOR) {
            objc_msgSend_id(class("NSCursor"), sel("unhide"));
        }
    }
}

bool _tigrIsWindowClosed(id window) {
    id wdg = objc_msgSend_id(window, sel("delegate"));
    if (!wdg)
        return false;
    NSUInteger value = 0;
    object_getInstanceVariable(wdg, "closed", (void**)&value);
    return value ? true : false;
}

bool _tigrIsWindowVisible(id window) {
    id wdg = objc_msgSend_id(window, sel("delegate"));
    if (!wdg)
        return false;
    NSUInteger value = 0;
    object_getInstanceVariable(wdg, "visible", (void**)&value);
    return value ? true : false;
}

static bool tigrOSXInited = false;
static id autoreleasePool = NULL;

#ifdef DEBUG
static void _showPools(const char* context) {
    fprintf(stderr, "NSAutoreleasePool@%s:\n", context);
    objc_msgSend(class("NSAutoreleasePool"), sel("showPools"));
}
#define showPools(x) _showPools((x))
#else
#define showPools(x)
#endif

static id pushPool(void) {
    id pool = objc_msgSend_id(class("NSAutoreleasePool"), sel("alloc"));
    return objc_msgSend_id(pool, sel("init"));
}

static void popPool(id pool) {
    objc_msgSend_void(pool, sel("drain"));
}

void _tigrCleanupOSX(void) {
    showPools("cleanup");
    popPool(autoreleasePool);
}

void tigrInitOSX(void) {
    if (tigrOSXInited)
        return;

    atexit(&_tigrCleanupOSX);

    autoreleasePool = pushPool();

    showPools("init start");

    objc_msgSend_id(class("NSApplication"), sel("sharedApplication"));
    objc_msgSend_t(void, NSInteger)(NSApp, sel("setActivationPolicy:"), NSApplicationActivationPolicyRegular);

    Class appDelegateClass = makeClass("AppDelegate", "NSObject");
    addMethod(appDelegateClass, "applicationShouldTerminate", applicationShouldTerminate, NSUIntegerEncoding "@:@");
    id dgAlloc = objc_msgSend_id((id)appDelegateClass, sel("alloc"));
    id dg = objc_msgSend_id(dgAlloc, sel("init"));

    objc_msgSend_void_id(NSApp, sel("setDelegate:"), dg);
    objc_msgSend_void(NSApp, sel("finishLaunching"));

    id menuBar = objc_alloc("NSMenu");
    menuBar = objc_msgSend_id(menuBar, sel("init"));

    id appMenuItem = objc_alloc("NSMenuItem");
    appMenuItem = objc_msgSend_id(appMenuItem, sel("init"));

    objc_msgSend_void_id(menuBar, sel("addItem:"), appMenuItem);
    objc_msgSend_t(id, id)(NSApp, sel("setMainMenu:"), menuBar);

    id processInfo = objc_msgSend_id(class("NSProcessInfo"), sel("processInfo"));
    id appName = objc_msgSend_id(processInfo, sel("processName"));

    id appMenu = objc_alloc("NSMenu");
    appMenu = objc_msgSend_t(id, id)(appMenu, sel("initWithTitle:"), appName);

    id quitTitlePrefixString =
        objc_msgSend_t(id, const char*)(class("NSString"), sel("stringWithUTF8String:"), "Quit ");
    id quitTitle = objc_msgSend_t(id, id)(quitTitlePrefixString, sel("stringByAppendingString:"), appName);

    id quitMenuItemKey = objc_msgSend_t(id, const char*)(class("NSString"), sel("stringWithUTF8String:"), "q");
    id quitMenuItem = objc_alloc("NSMenuItem");
    quitMenuItem = objc_msgSend_t(id, id, SEL, id)(quitMenuItem, sel("initWithTitle:action:keyEquivalent:"), quitTitle,
                                                   sel("terminate:"), quitMenuItemKey);

    objc_msgSend_void_id(appMenu, sel("addItem:"), quitMenuItem);
    objc_msgSend_void_id(appMenuItem, sel("setSubmenu:"), appMenu);

    tigrOSXInited = true;

    showPools("init end");
}

void tigrError(Tigr* bmp, const char* message, ...) {
    char tmp[1024];

    va_list args;
    va_start(args, message);
    vsnprintf(tmp, sizeof(tmp), message, args);
    tmp[sizeof(tmp) - 1] = 0;
    va_end(args);

    printf("tigr fatal error: %s\n", tmp);

    exit(1);
}

NSSize _tigrContentBackingSize(id window) {
    id contentView = objc_msgSend_id(window, sel("contentView"));
    NSRect rect = objc_msgSend_stret_t(NSRect)(contentView, sel("frame"));
    rect = objc_msgSend_stret_t(NSRect, NSRect)(contentView, sel("convertRectToBacking:"), rect);

    return rect.size;
}

enum {
    NSWindowStyleMaskTitled = 1 << 0,
    NSWindowStyleMaskClosable = 1 << 1,
    NSWindowStyleMaskMiniaturizable = 1 << 2,
    NSWindowStyleMaskResizable = 1 << 3,
    NSWindowStyleRegular = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                           NSWindowStyleMaskResizable,
    NSWindowStyleMaskFullSizeContentView = 1 << 15
};

Tigr* tigrWindow(int w, int h, const char* title, int flags) {
    Tigr* bmp;
    TigrInternal* win;

    tigrInitOSX();

    NSUInteger windowStyleMask = NSWindowStyleRegular & ~NSWindowStyleMaskMiniaturizable;

    // In AUTO mode, window follows requested size, unless downscaled by tigrEnforceScale below.
    int windowScale = 1;

    // In non-AUTO mode, see how big we can make it and still fit on-screen.
    if ((flags & TIGR_AUTO) == 0) {
        CGRect mainMonitor = CGDisplayBounds(CGMainDisplayID());
        int maxW = CGRectGetWidth(mainMonitor);
        int maxH = CGRectGetHeight(mainMonitor);
        NSRect screen = { { 0, 0 }, { maxW, maxH } };
        NSRect content = objc_msgSend_stret_t(NSRect, NSRect, NSUInteger)(
            class("NSWindow"), sel("contentRectForFrameRect:styleMask:"), screen, windowStyleMask);
        windowScale = tigrCalcScale(w, h, content.size.width, content.size.height);
    }

    windowScale = tigrEnforceScale(windowScale, flags);

    NSRect rect = { { 0, 0 }, { w * windowScale, h * windowScale } };
    id windowAlloc = objc_msgSend_id(class("NSWindow"), sel("alloc"));
    id window = ((id(*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(
        windowAlloc, sel("initWithContentRect:styleMask:backing:defer:"), rect, windowStyleMask, 2, NO);

    objc_msgSend_void_bool(window, sel("setReleasedWhenClosed:"), NO);

    Class WindowDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "WindowDelegate", 0);
    addIvar(WindowDelegateClass, "closed", sizeof(NSUInteger), NSUIntegerEncoding);
    addIvar(WindowDelegateClass, "visible", sizeof(NSUInteger), NSUIntegerEncoding);
    addIvar(WindowDelegateClass, "tigrHandle", sizeof(void*), "ˆv");
    addMethod(WindowDelegateClass, "windowWillClose:", windowWillClose, "v@:@");
    addMethod(WindowDelegateClass, "windowDidEnterFullScreen:", windowDidEnterFullScreen, "v@:@");
    addMethod(WindowDelegateClass, "windowDidResize:", windowDidResize, "v@:@");
    addMethod(WindowDelegateClass, "windowDidBecomeKey:", windowDidBecomeKey, "v@:@");
    addMethod(WindowDelegateClass, "mouseEntered:", mouseEntered, "v@:@");
    addMethod(WindowDelegateClass, "mouseExited:", mouseExited, "v@:@");

    id wdgAlloc = objc_msgSend_id((id)WindowDelegateClass, sel("alloc"));
    id wdg = objc_msgSend_id(wdgAlloc, sel("init"));

    if (flags & TIGR_FULLSCREEN) {
        objc_msgSend_void_id(window, sel("toggleFullScreen:"), window);
        if (flags & TIGR_NOCURSOR) {
            objc_msgSend_id(class("NSCursor"), sel("hide"));
        }
    } else {
        NSUInteger value = true;
        object_setInstanceVariable(wdg, "visible", (void*)value);
    }

    objc_msgSend_void_id(window, sel("setDelegate:"), wdg);

    id contentView = objc_msgSend_id(window, sel("contentView"));

    int wantsHighRes = (flags & TIGR_RETINA);
    objc_msgSend_void_bool(contentView, sel("setWantsBestResolutionOpenGLSurface:"), wantsHighRes);

    NSPoint point = { 20, 20 };
    ((void (*)(id, SEL, NSPoint))objc_msgSend)(window, sel("cascadeTopLeftFromPoint:"), point);

    id titleString = objc_msgSend_t(id, const char*)(class("NSString"), sel("stringWithUTF8String:"), title);
    objc_msgSend_void_id(window, sel("setTitle:"), titleString);

    uint32_t glAttributes[] = { 8, 24,  //	NSOpenGLPFAColorSize, 24,
                                11, 8,  //	NSOpenGLPFAAlphaSize, 8,
                                5,      //	NSOpenGLPFADoubleBuffer,
                                73,     //	NSOpenGLPFAAccelerated,
                                // 72,			//	NSOpenGLPFANoRecovery,
                                // 55, 1,		//	NSOpenGLPFASampleBuffers, 1,
                                // 56, 4,		//	NSOpenGLPFASamples, 4,
                                99, 0x3200,  //	NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
                                // 70, 0x00020400, // NSOpenGLPFARendererID, kCGLRendererGenericFloatID
                                0 };

    id pixelFormat = objc_alloc("NSOpenGLPixelFormat");
    pixelFormat = objc_msgSend_t(id, const uint32_t*)(pixelFormat, sel("initWithAttributes:"), glAttributes);
    objc_msgSend_void(pixelFormat, sel("autorelease"));

    id openGLContext = objc_alloc("NSOpenGLContext");
    openGLContext = objc_msgSend_t(id, id, id)(openGLContext, sel("initWithFormat:shareContext:"), pixelFormat, nil);

    objc_msgSend_void_id(openGLContext, sel("setView:"), contentView);
    objc_msgSend_void_id(window, sel("makeKeyAndOrderFront:"), window);
    objc_msgSend_void_bool(window, sel("setAcceptsMouseMovedEvents:"), YES);

    id blackColor = objc_msgSend_id(class("NSColor"), sel("blackColor"));
    objc_msgSend_void_id(window, sel("setBackgroundColor:"), blackColor);

    objc_msgSend_void_bool(NSApp, sel("activateIgnoringOtherApps:"), YES);

    NSSize windowContentSize = _tigrContentBackingSize(window);

    // In AUTO mode, always use a 1:1 pixel size, unless downscaled by tigrEnforceScale below.
    int bitmapScale = 1;

    // In non-AUTO mode, scale based on backing size
    if ((flags & TIGR_AUTO) == 0) {
        bitmapScale = tigrEnforceScale(tigrCalcScale(w, h, windowContentSize.width, windowContentSize.height), flags);
    } else {
        // In AUTO mode, bitmap size follows window size
        w = windowContentSize.width / windowScale;
        h = windowContentSize.height / windowScale;
        bitmapScale = tigrEnforceScale(bitmapScale, flags);
    }

    bmp = tigrBitmap2(w, h, sizeof(TigrInternal));
    bmp->handle = window;

    // Set the handle
    object_setInstanceVariable(wdg, "tigrHandle", (void*)bmp);

    {
#define NSTrackingMouseEnteredAndExited 1
#define NSTrackingActiveInKeyWindow 0x20
#define NSTrackingInVisibleRect 0x200

        int trackingFlags = NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow | NSTrackingInVisibleRect;
        id trackingArea = objc_msgSend_id(class("NSTrackingArea"), sel("alloc"));
        trackingArea = objc_msgSend_t(id, NSRect, int, id, id)(
            trackingArea, sel("initWithRect:options:owner:userInfo:"), rect, trackingFlags, wdg, 0);
        objc_msgSend_void_id(contentView, sel("addTrackingArea:"), trackingArea);
    }

    // Set up the Windows parts.
    win = tigrInternal(bmp);
    win->shown = 0;
    win->closed = 0;
    win->scale = bitmapScale;
    win->lastChar = 0;
    win->flags = flags;
    win->p1 = win->p2 = win->p3 = 0;
    win->p4 = 1;
    win->widgetsWanted = 0;
    win->widgetAlpha = 0;
    win->widgetsScale = 0;
    win->widgets = 0;
    win->gl.gl_legacy = 0;
    win->gl.glContext = openGLContext;
    win->mouseButtons = 0;
    win->mouseInView = 0;

    tigrPosition(bmp, win->scale, bmp->w, bmp->h, win->pos);

    objc_msgSend_void(openGLContext, sel("makeCurrentContext"));
    tigrGAPICreate(bmp);

    return bmp;
}

void tigrFree(Tigr* bmp) {
    if (bmp->handle) {
        TigrInternal* win = tigrInternal(bmp);
        tigrGAPIDestroy(bmp);

        id window = (id)bmp->handle;

        if (!_tigrIsWindowClosed(window) && !terminated) {
            objc_msgSend_void(window, sel("close"));
        }

        if (win->flags & TIGR_NOCURSOR) {
            objc_msgSend_id(class("NSCursor"), sel("unhide"));
        }

        id wdg = objc_msgSend_id(window, sel("delegate"));
        objc_msgSend_void(wdg, sel("release"));
        objc_msgSend_void((id)win->gl.glContext, sel("release"));
        objc_msgSend_void(window, sel("release"));
    }
    free(bmp->pix);
    free(bmp);
}

uint8_t _tigrKeyFromOSX(uint16_t key) {
    // from Carbon HIToolbox/Events.h
    enum {
        kVK_ANSI_A = 0x00,
        kVK_ANSI_S = 0x01,
        kVK_ANSI_D = 0x02,
        kVK_ANSI_F = 0x03,
        kVK_ANSI_H = 0x04,
        kVK_ANSI_G = 0x05,
        kVK_ANSI_Z = 0x06,
        kVK_ANSI_X = 0x07,
        kVK_ANSI_C = 0x08,
        kVK_ANSI_V = 0x09,
        kVK_ANSI_B = 0x0B,
        kVK_ANSI_Q = 0x0C,
        kVK_ANSI_W = 0x0D,
        kVK_ANSI_E = 0x0E,
        kVK_ANSI_R = 0x0F,
        kVK_ANSI_Y = 0x10,
        kVK_ANSI_T = 0x11,
        kVK_ANSI_1 = 0x12,
        kVK_ANSI_2 = 0x13,
        kVK_ANSI_3 = 0x14,
        kVK_ANSI_4 = 0x15,
        kVK_ANSI_6 = 0x16,
        kVK_ANSI_5 = 0x17,
        kVK_ANSI_Equal = 0x18,
        kVK_ANSI_9 = 0x19,
        kVK_ANSI_7 = 0x1A,
        kVK_ANSI_Minus = 0x1B,
        kVK_ANSI_8 = 0x1C,
        kVK_ANSI_0 = 0x1D,
        kVK_ANSI_RightBracket = 0x1E,
        kVK_ANSI_O = 0x1F,
        kVK_ANSI_U = 0x20,
        kVK_ANSI_LeftBracket = 0x21,
        kVK_ANSI_I = 0x22,
        kVK_ANSI_P = 0x23,
        kVK_ANSI_L = 0x25,
        kVK_ANSI_J = 0x26,
        kVK_ANSI_Quote = 0x27,
        kVK_ANSI_K = 0x28,
        kVK_ANSI_Semicolon = 0x29,
        kVK_ANSI_Backslash = 0x2A,
        kVK_ANSI_Comma = 0x2B,
        kVK_ANSI_Slash = 0x2C,
        kVK_ANSI_N = 0x2D,
        kVK_ANSI_M = 0x2E,
        kVK_ANSI_Period = 0x2F,
        kVK_ANSI_Grave = 0x32,
        kVK_ANSI_KeypadDecimal = 0x41,
        kVK_ANSI_KeypadMultiply = 0x43,
        kVK_ANSI_KeypadPlus = 0x45,
        kVK_ANSI_KeypadClear = 0x47,
        kVK_ANSI_KeypadDivide = 0x4B,
        kVK_ANSI_KeypadEnter = 0x4C,
        kVK_ANSI_KeypadMinus = 0x4E,
        kVK_ANSI_KeypadEquals = 0x51,
        kVK_ANSI_Keypad0 = 0x52,
        kVK_ANSI_Keypad1 = 0x53,
        kVK_ANSI_Keypad2 = 0x54,
        kVK_ANSI_Keypad3 = 0x55,
        kVK_ANSI_Keypad4 = 0x56,
        kVK_ANSI_Keypad5 = 0x57,
        kVK_ANSI_Keypad6 = 0x58,
        kVK_ANSI_Keypad7 = 0x59,
        kVK_ANSI_Keypad8 = 0x5B,
        kVK_ANSI_Keypad9 = 0x5C,
        kVK_Return = 0x24,
        kVK_Tab = 0x30,
        kVK_Space = 0x31,
        kVK_Delete = 0x33,
        kVK_Escape = 0x35,
        kVK_Command = 0x37,
        kVK_Shift = 0x38,
        kVK_CapsLock = 0x39,
        kVK_Option = 0x3A,
        kVK_Control = 0x3B,
        kVK_RightShift = 0x3C,
        kVK_RightOption = 0x3D,
        kVK_RightControl = 0x3E,
        kVK_Function = 0x3F,
        kVK_F17 = 0x40,
        kVK_VolumeUp = 0x48,
        kVK_VolumeDown = 0x49,
        kVK_Mute = 0x4A,
        kVK_F18 = 0x4F,
        kVK_F19 = 0x50,
        kVK_F20 = 0x5A,
        kVK_F5 = 0x60,
        kVK_F6 = 0x61,
        kVK_F7 = 0x62,
        kVK_F3 = 0x63,
        kVK_F8 = 0x64,
        kVK_F9 = 0x65,
        kVK_F11 = 0x67,
        kVK_F13 = 0x69,
        kVK_F16 = 0x6A,
        kVK_F14 = 0x6B,
        kVK_F10 = 0x6D,
        kVK_F12 = 0x6F,
        kVK_F15 = 0x71,
        kVK_Help = 0x72,
        kVK_Home = 0x73,
        kVK_PageUp = 0x74,
        kVK_ForwardDelete = 0x75,
        kVK_F4 = 0x76,
        kVK_End = 0x77,
        kVK_F2 = 0x78,
        kVK_PageDown = 0x79,
        kVK_F1 = 0x7A,
        kVK_LeftArrow = 0x7B,
        kVK_RightArrow = 0x7C,
        kVK_DownArrow = 0x7D,
        kVK_UpArrow = 0x7E
    };

    switch (key) {
        case kVK_ANSI_Q:
            return 'Q';
        case kVK_ANSI_W:
            return 'W';
        case kVK_ANSI_E:
            return 'E';
        case kVK_ANSI_R:
            return 'R';
        case kVK_ANSI_T:
            return 'T';
        case kVK_ANSI_Y:
            return 'Y';
        case kVK_ANSI_U:
            return 'U';
        case kVK_ANSI_I:
            return 'I';
        case kVK_ANSI_O:
            return 'O';
        case kVK_ANSI_P:
            return 'P';
        case kVK_ANSI_A:
            return 'A';
        case kVK_ANSI_S:
            return 'S';
        case kVK_ANSI_D:
            return 'D';
        case kVK_ANSI_F:
            return 'F';
        case kVK_ANSI_G:
            return 'G';
        case kVK_ANSI_H:
            return 'H';
        case kVK_ANSI_J:
            return 'J';
        case kVK_ANSI_K:
            return 'K';
        case kVK_ANSI_L:
            return 'L';
        case kVK_ANSI_Z:
            return 'Z';
        case kVK_ANSI_X:
            return 'X';
        case kVK_ANSI_C:
            return 'C';
        case kVK_ANSI_V:
            return 'V';
        case kVK_ANSI_B:
            return 'B';
        case kVK_ANSI_N:
            return 'N';
        case kVK_ANSI_M:
            return 'M';
        case kVK_ANSI_0:
            return '0';
        case kVK_ANSI_1:
            return '1';
        case kVK_ANSI_2:
            return '2';
        case kVK_ANSI_3:
            return '3';
        case kVK_ANSI_4:
            return '4';
        case kVK_ANSI_5:
            return '5';
        case kVK_ANSI_6:
            return '6';
        case kVK_ANSI_7:
            return '7';
        case kVK_ANSI_8:
            return '8';
        case kVK_ANSI_9:
            return '9';
        case kVK_ANSI_Keypad0:
            return TK_PAD0;
        case kVK_ANSI_Keypad1:
            return TK_PAD1;
        case kVK_ANSI_Keypad2:
            return TK_PAD2;
        case kVK_ANSI_Keypad3:
            return TK_PAD3;
        case kVK_ANSI_Keypad4:
            return TK_PAD4;
        case kVK_ANSI_Keypad5:
            return TK_PAD5;
        case kVK_ANSI_Keypad6:
            return TK_PAD6;
        case kVK_ANSI_Keypad7:
            return TK_PAD7;
        case kVK_ANSI_Keypad8:
            return TK_PAD8;
        case kVK_ANSI_Keypad9:
            return TK_PAD9;
        case kVK_ANSI_KeypadMultiply:
            return TK_PADMUL;
        case kVK_ANSI_KeypadPlus:
            return TK_PADADD;
        case kVK_ANSI_KeypadEnter:
            return TK_PADENTER;
        case kVK_ANSI_KeypadMinus:
            return TK_PADSUB;
        case kVK_ANSI_KeypadDecimal:
            return TK_PADDOT;
        case kVK_ANSI_KeypadDivide:
            return TK_PADDIV;
        case kVK_F1:
            return TK_F1;
        case kVK_F2:
            return TK_F2;
        case kVK_F3:
            return TK_F3;
        case kVK_F4:
            return TK_F4;
        case kVK_F5:
            return TK_F5;
        case kVK_F6:
            return TK_F6;
        case kVK_F7:
            return TK_F7;
        case kVK_F8:
            return TK_F8;
        case kVK_F9:
            return TK_F9;
        case kVK_F10:
            return TK_F10;
        case kVK_F11:
            return TK_F11;
        case kVK_F12:
            return TK_F12;
        case kVK_Shift:
            return TK_LSHIFT;
        case kVK_Control:
            return TK_LCONTROL;
        case kVK_Option:
            return TK_LALT;
        case kVK_CapsLock:
            return TK_CAPSLOCK;
        case kVK_Command:
            return TK_LWIN;
        case kVK_Command - 1:
            return TK_RWIN;
        case kVK_RightShift:
            return TK_RSHIFT;
        case kVK_RightControl:
            return TK_RCONTROL;
        case kVK_RightOption:
            return TK_RALT;
        case kVK_Delete:
            return TK_BACKSPACE;
        case kVK_Tab:
            return TK_TAB;
        case kVK_Return:
            return TK_RETURN;
        case kVK_Escape:
            return TK_ESCAPE;
        case kVK_Space:
            return TK_SPACE;
        case kVK_PageUp:
            return TK_PAGEUP;
        case kVK_PageDown:
            return TK_PAGEDN;
        case kVK_End:
            return TK_END;
        case kVK_Home:
            return TK_HOME;
        case kVK_LeftArrow:
            return TK_LEFT;
        case kVK_UpArrow:
            return TK_UP;
        case kVK_RightArrow:
            return TK_RIGHT;
        case kVK_DownArrow:
            return TK_DOWN;
        case kVK_Help:
            return TK_INSERT;
        case kVK_ForwardDelete:
            return TK_DELETE;
        case kVK_F14:
            return TK_SCROLL;
        case kVK_F15:
            return TK_PAUSE;
        case kVK_ANSI_KeypadClear:
            return TK_NUMLOCK;
        case kVK_ANSI_Semicolon:
            return TK_SEMICOLON;
        case kVK_ANSI_Equal:
            return TK_EQUALS;
        case kVK_ANSI_Comma:
            return TK_COMMA;
        case kVK_ANSI_Minus:
            return TK_MINUS;
        case kVK_ANSI_Slash:
            return TK_SLASH;
        case kVK_ANSI_Backslash:
            return TK_BACKSLASH;
        case kVK_ANSI_Grave:
            return TK_BACKTICK;
        case kVK_ANSI_Quote:
            return TK_TICK;
        case kVK_ANSI_LeftBracket:
            return TK_LSQUARE;
        case kVK_ANSI_RightBracket:
            return TK_RSQUARE;
        case kVK_ANSI_Period:
            return TK_DOT;
        default:
            return 0;
    }
}

void _tigrOnCocoaEvent(id event, id window) {
    if (!event)
        return;

    TigrInternal* win = _tigrInternalCocoa(window);
    if (!win)  // just pipe the event
    {
        objc_msgSend_void_id(NSApp, sel("sendEvent:"), event);
        return;
    }

    NSUInteger eventType = objc_msgSend_t(NSUInteger)(event, sel("type"));
    switch (eventType) {
        case 1:  // NSLeftMouseDown
            if (win->mouseInView) {
                win->mouseButtons |= 1;
            }
            break;
        case 2:  // NSLeftMouseUp
            win->mouseButtons &= ~1;
            break;
        case 3:  // NSRightMouseDown
            if (win->mouseInView) {
                win->mouseButtons |= 2;
            }
            break;
        case 4:  // NSRightMouseUp
            win->mouseButtons &= ~2;
            break;
        case 25:  // NSOtherMouseDown
        {
            // number == 2 is a middle button
            NSInteger number = objc_msgSend_t(NSInteger)(event, sel("buttonNumber"));
            if (number == 2 && win->mouseInView) {
                win->mouseButtons |= 4;
            }
            break;
        }
        case 26:  // NSOtherMouseUp
        {
            NSInteger number = objc_msgSend_t(NSInteger)(event, sel("buttonNumber"));
            if (number == 2)
                win->mouseButtons &= ~4;
            break;
        }
        case 12:  // NSFlagsChanged
        {
            NSUInteger modifiers = objc_msgSend_t(NSUInteger)(event, sel("modifierFlags"));

            // based on NSEventModifierFlags and
            // NSDeviceIndependentModifierFlagsMask
            struct {
                union {
                    struct {
                        uint8_t alpha_shift : 1;
                        uint8_t shift : 1;
                        uint8_t control : 1;
                        uint8_t alternate : 1;
                        uint8_t command : 1;
                        uint8_t numeric_pad : 1;
                        uint8_t help : 1;
                        uint8_t function : 1;
                    };
                    uint8_t mask;
                };
            } keys;

            keys.mask = (modifiers & 0xffff0000UL) >> 16;

            // TODO L,R variation of keys?
            win->keys[TK_CONTROL] = keys.alpha_shift;
            win->keys[TK_SHIFT] = keys.shift;
            win->keys[TK_CONTROL] = keys.control;
            win->keys[TK_ALT] = keys.alternate;
            win->keys[TK_LWIN] = keys.command;
            win->keys[TK_RWIN] = keys.command;
            break;
        }
        case 10:  // NSKeyDown
        {
            uint16_t keyCode = objc_msgSend_t(unsigned short)(event, sel("keyCode"));
            int tigrKey = _tigrKeyFromOSX(keyCode);

            // Ignore keyboard repeats
            if (!win->keys[tigrKey]) {
                win->keys[tigrKey] = 1;
                id inputText = objc_msgSend_id(event, sel("characters"));
                const char* inputTextUTF8 = objc_msgSend_t(const char*)(inputText, sel("UTF8String"));

                int decoded = 0;
                tigrDecodeUTF8(inputTextUTF8, &decoded);
                if (decoded < 0xe000 || decoded > 0xf8ff) {
                    win->lastChar = decoded;
                }
            }

            // Pass through cmd+key
            if (win->keys[TK_LWIN]) {
                break;
            }
            return;
        }
        case 11:  // NSKeyUp
        {
            uint16_t keyCode = objc_msgSend_t(unsigned short)(event, sel("keyCode"));
            win->keys[_tigrKeyFromOSX(keyCode)] = 0;
            return;
        }
        default:
            break;
    }

    objc_msgSend_void_id(NSApp, sel("sendEvent:"), event);
}

void tigrUpdate(Tigr* bmp) {
    popPool(autoreleasePool);
    autoreleasePool = pushPool();

    TigrInternal* win;
    id openGLContext;
    id window;
    win = tigrInternal(bmp);
    window = (id)bmp->handle;
    openGLContext = (id)win->gl.glContext;

    if (terminated || _tigrIsWindowClosed(window)) {
        return;
    }

    id keyWindow = objc_msgSend_id(NSApp, sel("keyWindow"));
    unsigned long long eventMask = NSAllEventMask;

    if (keyWindow == window) {
        memcpy(win->prev, win->keys, 256);
    } else {
        eventMask &= ~(NSKeyDownMask | NSKeyUpMask);
    }

    id event = 0;
    BOOL visible = 0;

    uint64_t now = mach_absolute_time();
    uint64_t passed = now - tigrTimestamp;

    do {
        event =
            objc_msgSend_t(id, NSUInteger, id, id, BOOL)(NSApp, sel("nextEventMatchingMask:untilDate:inMode:dequeue:"),
                                                         eventMask, nil, NSDefaultRunLoopMode, YES);

        if (event != 0) {
            _tigrOnCocoaEvent(event, window);
        } else {
            visible = _tigrIsWindowVisible(window);
        }
    } while (event != 0 || !visible);

    // The event processing loop above blocks during resize, which causes updates to freeze
    // but real time keeps ticking. We pretend that the event processing took no time
    // to avoid huge jumps in tigrTime.
    tigrTimestamp = mach_absolute_time() - passed;

    // do runloop stuff
    objc_msgSend_void(NSApp, sel("updateWindows"));
    objc_msgSend_void(openGLContext, sel("update"));
    tigrGAPIBegin(bmp);

    NSSize windowSize = _tigrContentBackingSize(window);

    if (win->flags & TIGR_AUTO)
        tigrResize(bmp, windowSize.width / win->scale, windowSize.height / win->scale);
    else
        win->scale = tigrEnforceScale(tigrCalcScale(bmp->w, bmp->h, windowSize.width, windowSize.height), win->flags);

    tigrPosition(bmp, win->scale, windowSize.width, windowSize.height, win->pos);
    tigrGAPIPresent(bmp, windowSize.width, windowSize.height);
    objc_msgSend_void(openGLContext, sel("flushBuffer"));
    tigrGAPIEnd(bmp);
}

int tigrGAPIBegin(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    objc_msgSend_void((id)win->gl.glContext, sel("makeCurrentContext"));
    return 0;
}

int tigrGAPIEnd(Tigr* bmp) {
    (void)bmp;
    objc_msgSend_void(class("NSOpenGLContext"), sel("clearCurrentContext"));
    return 0;
}

int tigrClosed(Tigr* bmp) {
    return (terminated || _tigrIsWindowClosed((id)bmp->handle)) ? 1 : 0;
}

void tigrMouse(Tigr* bmp, int* x, int* y, int* buttons) {
    TigrInternal* win;
    id window;
    win = tigrInternal(bmp);
    window = (id)bmp->handle;

    id windowContentView = objc_msgSend_id(window, sel("contentView"));
    NSRect adjustFrame = objc_msgSend_stret_t(NSRect)(windowContentView, sel("frame"));

    // NSPoint is small enough to fit a register, so no need for
    // objc_msgSend_stret
    NSPoint p = objc_msgSend_t(NSPoint)(window, sel("mouseLocationOutsideOfEventStream"));

    // map input to content view rect
    if (p.x < 0)
        p.x = 0;
    else if (p.x > adjustFrame.size.width)
        p.x = adjustFrame.size.width;
    if (p.y < 0)
        p.y = 0;
    else if (p.y > adjustFrame.size.height)
        p.y = adjustFrame.size.height;

    // map input to pixels
    NSRect r = { p, { 0, 0 } };
    r = objc_msgSend_stret_t(NSRect, NSRect)(windowContentView, sel("convertRectToBacking:"), r);
    p = r.origin;

    p.x = (p.x - win->pos[0]) / win->scale;
    p.y = bmp->h - (p.y - win->pos[1]) / win->scale;

    if (x)
        *x = p.x;
    if (y)
        *y = p.y;

    if (buttons) {
        id keyWindow = objc_msgSend_id(NSApp, sel("keyWindow"));
        *buttons = keyWindow != bmp->handle ? 0 : win->mouseButtons;
    }
}

int tigrTouch(Tigr* bmp, TigrTouchPoint* points, int maxPoints) {
    int buttons = 0;
    if (maxPoints > 0) {
        tigrMouse(bmp, &points[0].x, &points[1].y, &buttons);
    }
    return buttons ? 1 : 0;
}

int tigrKeyDown(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return (win->keys[key] != 0) && (win->prev[key] == 0);
}

int tigrKeyHeld(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return win->keys[key];
}

int tigrReadChar(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    int c = win->lastChar;
    win->lastChar = 0;
    return c;
}

float tigrTime(void) {
    static mach_timebase_info_data_t timebaseInfo;

    if (timebaseInfo.denom == 0) {
        mach_timebase_info(&timebaseInfo);
        tigrTimestamp = mach_absolute_time();
        return 0.0f;
    }

    uint64_t current_time = mach_absolute_time();
    double elapsed = (double)(current_time - tigrTimestamp) * timebaseInfo.numer / (timebaseInfo.denom * 1000000000.0);
    tigrTimestamp = current_time;
    return (float)elapsed;
}

#endif // __MACOS__
#endif // #ifndef TIGR_HEADLESS

//////// End of inlined file: tigr_osx.c ////////

//////// Start of inlined file: tigr_ios.c ////////

#ifndef TIGR_HEADLESS

//#include "tigr_internal.h"
//#include "tigr_objc.h"

#ifdef __IOS__

#include <CoreGraphics/CoreGraphics.h>
#include <objc/message.h>
#include <objc/objc.h>
#include <objc/runtime.h>
#include <dispatch/dispatch.h>
#include <os/log.h>
#include <time.h>
#include <stdatomic.h>

id makeNSString(const char* str) {
    return objc_msgSend_t(id, const char*)(class("NSString"), sel("stringWithUTF8String:"), str);
}

id joinNSStrings(id a, id b) {
    return objc_msgSend_t(id, id)(a, sel("stringByAppendingString:"), b);
}

const char* UTF8StringFromNSString(id a) {
    return objc_msgSend_t(const char*)(a, sel("UTF8String"));
}

extern id UIApplication;
static int NSQualityOfServiceUserInteractive = 0x21;

typedef struct {
    TigrTouchPoint points[MAX_TOUCH_POINTS];
    int numPoints;
} InputState;

typedef struct {
    int keyCode;
    int codePoint;
} KeyEvent;

enum {
    KBD_HIDDEN = 0,
    KBD_SHOWREQ,
    KBD_HIDEREQ,
    KBD_SHOWN,
};

/// Global state
static struct {
    InputState inputState;
    id viewController;
    id view;
    id context;
    id frameCondition;
    int screenW;
    int screenH;
    double scaleFactor;
    double timeSinceLastDraw;
    int renderReadFd;
    int mainWriteFd;
    _Atomic(int) keyboardState;
} gState = {
    .inputState = {
        .numPoints = 0,
    },
    .viewController = 0,
    .view = 0,
    .context = 0,
    .frameCondition = 0,
    .screenW = 0,
    .screenH = 0,
    .scaleFactor = 1,
    .timeSinceLastDraw = 0,
    .renderReadFd = 0,
    .mainWriteFd = 0,
    .keyboardState = ATOMIC_VAR_INIT(KBD_HIDDEN),
};

typedef enum {
    SET_INPUT,
    KEY_EVENT,
} TigrMessage;

typedef struct TigrMessageData {
    TigrMessage message;
    union {
        InputState inputState;
        KeyEvent keyEvent;
    };
} TigrMessageData;

static id autoreleasePool = NULL;

void writeToRenderThread(const TigrMessageData* message) {
    if (write(gState.mainWriteFd, message, sizeof(TigrMessageData)) != sizeof(TigrMessageData)) {
        os_log_error(OS_LOG_DEFAULT, "Failed to write message to render thread: %{public}s", strerror(errno));
    }
}

int readFromMainThread(TigrMessageData* message) {
    int result = read(gState.renderReadFd, message, sizeof(TigrMessageData));
    if (result == -1 && errno == EAGAIN) {
        return 0;
    }
    if (result != sizeof(TigrMessageData)) {
        os_log_error(OS_LOG_DEFAULT, "Failed to read message from main thread: %{public}s", strerror(errno));
        return 0;
    }
    return 1;
}

void viewWillTransitionToSize(id self, SEL _sel, CGSize size, id transitionCoordinator) {
    // No animation, just set them
    gState.screenW = size.width * gState.scaleFactor;
    gState.screenH = size.height * gState.scaleFactor;
    struct objc_super super = {
        self,
        objc_getClass("GLKViewController"),
    };
    objc_msgSendSuper_t(void, CGSize, id)(&super, _sel, size, transitionCoordinator);
}

BOOL prefersStatusBarHidden(id self, SEL _sel) {
    return YES;
}

BOOL hasText(id self, SEL _sel) {
    return NO;
}

void tigrShowKeyboard(int show) {
    int expected = show ? KBD_HIDDEN : KBD_SHOWN;
    int desired = show ? KBD_SHOWREQ : KBD_HIDEREQ;
    atomic_compare_exchange_weak(&gState.keyboardState, &expected, desired);
}

void insertText(id self, SEL _sel, id text) {
    const char* inserted = UTF8StringFromNSString(text);
    int codePoint = 0;

    do {
        inserted = tigrDecodeUTF8(inserted, &codePoint);
        if (codePoint != 0) {
            KeyEvent event;
            event.codePoint = codePoint;
            event.keyCode = (codePoint < 128) ? codePoint : 0;

            TigrMessageData message = {
                .message = KEY_EVENT,
                .keyEvent = event,
            };
            writeToRenderThread(&message);
        }
    } while (*inserted != 0);
}

void deleteBackward(id self, SEL _sel) {
    KeyEvent event;
    event.codePoint = 0;
    event.keyCode = 8;  // BS

    TigrMessageData message = { .message = KEY_EVENT,
                                .keyEvent = {
                                    .codePoint = 0,
                                    .keyCode = 8,
                                } };
    writeToRenderThread(&message);
}

BOOL canBecomeFirstResponder(id self, SEL _sel) {
    return YES;
}

BOOL canResignFirstResponder(id self, SEL _sel) {
    return YES;
}

enum RenderState { SWAPPED = 5150, RENDERED };

BOOL didFinishLaunchingWithOptions(id self, SEL _sel, id application, id options) {
    id screen = objc_msgSend_id(class("UIScreen"), sel("mainScreen"));
    CGRect bounds = objc_msgSend_t(CGRect)(screen, sel("bounds"));
    CGSize size = bounds.size;

    id window = objc_alloc("UIWindow");
    window = objc_msgSend_t(id, CGRect)(window, sel("initWithFrame:"), bounds);

    Class ViewController = makeClass("TigrViewController", "GLKViewController");
    addMethod(ViewController, "viewWillTransitionToSize:withTransitionCoordinator:", viewWillTransitionToSize,
              "v@:{CGSize}@");
    addMethod(ViewController, "prefersStatusBarHidden", prefersStatusBarHidden, "c@:");
    id vc = objc_msgSend_t(id)((id)ViewController, sel("alloc"));
    vc = objc_msgSend_id(vc, sel("init"));
    gState.viewController = vc;
    objc_msgSend_t(void, int)(vc, sel("setPreferredFramesPerSecond:"), 60);
    int framesPerSecond = objc_msgSend_t(int)(vc, sel("framesPerSecond"));

    id context = objc_alloc("EAGLContext");
    static int kEAGLRenderingAPIOpenGLES3 = 3;
    context = objc_msgSend_t(id, int)(context, sel("initWithAPI:"), kEAGLRenderingAPIOpenGLES3);
    gState.context = context;

    Class View = makeClass("TigrView", "GLKView");
    addMethod(View, "insertText:", insertText, "v@:@");
    addMethod(View, "deleteBackward", deleteBackward, "v@:");
    addMethod(View, "hasText", hasText, "c@:");
    addMethod(View, "canBecomeFirstResponder", canBecomeFirstResponder, "c@:");
    addMethod(View, "canResignFirstResponder", canResignFirstResponder, "c@:");

    Protocol* UIKeyInput = objc_getProtocol("UIKeyInput");
    class_addProtocol(View, UIKeyInput);

    id view = objc_msgSend_id((id)View, sel("alloc"));
    view = objc_msgSend_t(id, CGRect, id)(view, sel("initWithFrame:context:"), bounds, context);
    gState.view = view;
    objc_msgSend_t(void, BOOL)(view, sel("setMultipleTouchEnabled:"), YES);
    objc_msgSend_t(void, id)(view, sel("setDelegate:"), self);
    objc_msgSend_t(void, id)(vc, sel("setView:"), view);
    objc_msgSend_t(void, id)(vc, sel("setDelegate:"), self);
    objc_msgSend_t(void, id)(window, sel("setRootViewController:"), vc);
    objc_msgSend_t(void)(window, sel("makeKeyAndVisible"));

    gState.scaleFactor = objc_msgSend_t(double)(view, sel("contentScaleFactor"));
    gState.screenW = size.width * gState.scaleFactor;
    gState.screenH = size.height * gState.scaleFactor;

    gState.frameCondition = objc_msgSend_t(id, int)(objc_alloc("NSConditionLock"), sel("initWithCondition:"), RENDERED);
    objc_msgSend_t(void, int)(gState.frameCondition, sel("lockWhenCondition:"), RENDERED);

    id renderThread = objc_msgSend_t(id, id, SEL, id)(objc_alloc("NSThread"), sel("initWithTarget:selector:object:"),
                                                      self, sel("renderMain"), NULL);
    objc_msgSend_t(void, int)(renderThread, sel("setQualityOfService:"), NSQualityOfServiceUserInteractive);
    objc_msgSend_t(void, id)(renderThread, sel("setName:"), makeNSString("Tigr Render Thread"));
    objc_msgSend_void(renderThread, sel("start"));

    return YES;
}

void waitForFrame() {
    objc_msgSend_t(void, id)(class("EAGLContext"), sel("setCurrentContext:"), 0);
    objc_msgSend_t(void, int)(gState.frameCondition, sel("unlockWithCondition:"), RENDERED);
    objc_msgSend_t(void, int)(gState.frameCondition, sel("lockWhenCondition:"), SWAPPED);
    objc_msgSend_t(void, id)(class("EAGLContext"), sel("setCurrentContext:"), gState.context);
}

void processKeyboardRequest() {
    int showReq = KBD_SHOWREQ;
    int hideReq = KBD_HIDEREQ;

    if (atomic_compare_exchange_weak(&gState.keyboardState, &showReq, KBD_SHOWN)) {
        objc_msgSend_t(BOOL)(gState.view, sel("becomeFirstResponder"));
    } else if (atomic_compare_exchange_weak(&gState.keyboardState, &hideReq, KBD_HIDDEN)) {
        objc_msgSend_t(BOOL)(gState.view, sel("resignFirstResponder"));
    }
}

void drawInRect(id _self, SEL _sel, id view, CGRect rect) {
    gState.timeSinceLastDraw = objc_msgSend_t(double)(gState.viewController, sel("timeSinceLastDraw"));
    objc_msgSend_t(void, int)(gState.frameCondition, sel("unlockWithCondition:"), SWAPPED);
    objc_msgSend_t(void, int)(gState.frameCondition, sel("lockWhenCondition:"), RENDERED);
    objc_msgSend_t(void, id)(class("EAGLContext"), sel("setCurrentContext:"), gState.context);

    processKeyboardRequest();
}

extern void tigrMain();

void renderMain(id _self, SEL _sel) {
    objc_msgSend_t(void, int)(gState.frameCondition, sel("lockWhenCondition:"), SWAPPED);
    objc_msgSend_t(void, id)(class("EAGLContext"), sel("setCurrentContext:"), gState.context);
    tigrMain();
}

enum {
    UITouchPhaseBegan,
    UITouchPhaseMoved,
    UITouchPhaseStationary,
    UITouchPhaseEnded,
    UITouchPhaseCancelled,
};

void touches(id self, SEL sel, id touches, id event) {
    id allTouches = objc_msgSend_t(id)(event, sel("allTouches"));
    id enumerator = objc_msgSend_t(id)(allTouches, sel("objectEnumerator"));
    id touch = 0;
    InputState input = {
        .numPoints = 0,
    };
    while ((touch = objc_msgSend_t(id)(enumerator, sel("nextObject")))) {
        CGPoint location = objc_msgSend_t(CGPoint, id)(touch, sel("locationInView:"), NULL);
        int phase = objc_msgSend_t(int)(touch, sel("phase"));
        switch (phase) {
            case UITouchPhaseBegan:
            case UITouchPhaseMoved:
            case UITouchPhaseStationary:
                input.points[input.numPoints].x = location.x * gState.scaleFactor;
                input.points[input.numPoints].y = location.y * gState.scaleFactor;
                input.numPoints++;
                break;
        }
        if (input.numPoints >= MAX_TOUCH_POINTS) {
            break;
        }
    }
    TigrMessageData message = {
        .message = SET_INPUT,
        .inputState = input,
    };
    writeToRenderThread(&message);
}

Class tigrAppDelegate() {
    static Class delegateClass = 0;
    if (delegateClass != 0) {
        return delegateClass;
    }

    id application = objc_msgSend_id(class("UIApplication"), sel("sharedApplication"));
    delegateClass = makeClass("TigrAppDelegate", "UIResponder");
    addMethod(delegateClass, "application:didFinishLaunchingWithOptions:", didFinishLaunchingWithOptions, "c@:@@");
    addMethod(delegateClass, "touchesBegan:withEvent:", touches, "v@:@@");
    addMethod(delegateClass, "touchesMoved:withEvent:", touches, "v@:@@");
    addMethod(delegateClass, "touchesEnded:withEvent:", touches, "v@:@@");
    addMethod(delegateClass, "touchesCancelled:withEvent:", touches, "v@:@@");
    addMethod(delegateClass, "glkView:drawInRect:", drawInRect, "v@:@{CGRect}");
    addMethod(delegateClass, "renderMain", renderMain, "v@:");
    objc_registerClassPair(delegateClass);

    int fds[2];
    if (pipe(fds) != 0) {
        tigrError(0, "Failed to create message pipe");
    }
    int flags = fcntl(fds[0], F_GETFL, 0);
    fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);

    gState.renderReadFd = fds[0];
    gState.mainWriteFd = fds[1];
    return delegateClass;
}

Tigr* tigrWindow(int w, int h, const char* title, int flags) {
    int scale = 1;
    if (flags & TIGR_AUTO) {
        // Always use a 1:1 pixel size.
        scale = 1;
    } else {
        // See how big we can make it and still fit on-screen.
        scale = tigrCalcScale(w, h, gState.screenW, gState.screenH);
    }

    scale = tigrEnforceScale(scale, flags);
    Tigr* bmp = tigrBitmap2(w, h, sizeof(TigrInternal));
    bmp->handle = (void*)4711;
    TigrInternal* win = tigrInternal(bmp);
    win->shown = 0;
    win->closed = 0;
    win->scale = scale;

    win->lastChar = 0;
    win->flags = flags;
    win->p1 = win->p2 = win->p3 = 0;
    win->p4 = 1;
    win->widgetsWanted = 0;
    win->widgetAlpha = 0;
    win->widgetsScale = 0;
    win->widgets = 0;
    win->gl.gl_legacy = 0;

    tigrPosition(bmp, win->scale, bmp->w, bmp->h, win->pos);
    tigrGAPICreate(bmp);

    return bmp;
}

void processEvents(TigrInternal* win) {
    memset(win->keys, 0, 255);

    TigrMessageData data;

    while (readFromMainThread(&data)) {
        switch (data.message) {
            case SET_INPUT:
                gState.inputState = data.inputState;
                break;
            case KEY_EVENT:
                win->keys[data.keyEvent.keyCode] = 1;
                win->lastChar = data.keyEvent.codePoint;
                break;
        }
    }
}

static int toWindowX(TigrInternal* win, int x) {
    return (x - win->pos[0]) / win->scale;
}

static int toWindowY(TigrInternal* win, int y) {
    return (y - win->pos[1]) / win->scale;
}

void tigrUpdate(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);

    processEvents(win);

    win->numTouchPoints = gState.inputState.numPoints;
    for (int i = 0; i < win->numTouchPoints; i++) {
        win->touchPoints[i].x = toWindowX(win, gState.inputState.points[i].x);
        win->touchPoints[i].y = toWindowY(win, gState.inputState.points[i].y);
    }

    win->mouseButtons = win->numTouchPoints;
    if (win->mouseButtons > 0) {
        win->mouseX = win->touchPoints[0].x;
        win->mouseY = win->touchPoints[0].y;
    }

    if (win->flags & TIGR_AUTO) {
        tigrResize(bmp, gState.screenW / win->scale, gState.screenH / win->scale);
    } else {
        win->scale = tigrEnforceScale(tigrCalcScale(bmp->w, bmp->h, gState.screenW, gState.screenH), win->flags);
    }

    tigrPosition(bmp, win->scale, gState.screenW, gState.screenH, win->pos);
    tigrGAPIPresent(bmp, gState.screenW, gState.screenH);
    waitForFrame();
}

int tigrClosed(Tigr* bmp) {
    return 0;
}

void tigrError(Tigr* bmp, const char* message, ...) {
    char tmp[1024];

    va_list args;
    va_start(args, message);
    vsnprintf(tmp, sizeof(tmp), message, args);
    tmp[sizeof(tmp) - 1] = 0;
    va_end(args);

    os_log_error(OS_LOG_DEFAULT, "tigr fatal error: %{public}s\n", tmp);

    exit(1);
}

float tigrTime() {
    return (float)gState.timeSinceLastDraw;
}

void tigrMouse(Tigr* bmp, int* x, int* y, int* buttons) {
    TigrInternal* win = tigrInternal(bmp);
    if (x) {
        *x = win->mouseX;
    }
    if (y) {
        *y = win->mouseY;
    }
    if (buttons) {
        *buttons = win->mouseButtons;
    }
}

int tigrTouch(Tigr* bmp, TigrTouchPoint* points, int maxPoints) {
    TigrInternal* win = tigrInternal(bmp);
    for (int i = 0; i < maxPoints && i < win->numTouchPoints; i++) {
        points[i] = win->touchPoints[i];
    }
    return maxPoints < win->numTouchPoints ? maxPoints : win->numTouchPoints;
}

void tigrFree(Tigr* bmp) {
    if (bmp->handle) {
        TigrInternal* win = tigrInternal(bmp);
    }
    free(bmp->pix);
    free(bmp);
}

int tigrGAPIBegin(Tigr* bmp) {
    (void)bmp;
    return 0;
}

int tigrGAPIEnd(Tigr* bmp) {
    (void)bmp;
    return 0;
}

int tigrKeyDown(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return win->keys[key];
}

int tigrKeyHeld(Tigr* bmp, int key) {
    return tigrKeyDown(bmp, key);
}

int tigrReadChar(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    int c = win->lastChar;
    win->lastChar = 0;
    return c;
}

extern void* _tigrReadFile(const char* fileName, int* length);

void* tigrReadFile(const char* fileName, int* length) {
    id mainBundle = objc_msgSend_id(class("NSBundle"), sel("mainBundle"));
    id resourcePath = objc_msgSend_id(mainBundle, sel("resourcePath"));
    resourcePath = joinNSStrings(resourcePath, makeNSString("/"));
    resourcePath = joinNSStrings(resourcePath, makeNSString(fileName));
    return _tigrReadFile(UTF8StringFromNSString(resourcePath), length);
}

#endif  // __IOS__

#endif // #ifndef TIGR_HEADLESS

//////// End of inlined file: tigr_ios.c ////////

//////// Start of inlined file: tigr_android.h ////////

#ifndef TIGR_ANDROID_H
#define TIGR_ANDROID_H
#ifdef __ANDROID__

#include <android/input.h>
#include <android/native_window.h>
#include <EGL/egl.h>

typedef enum {
    AE_INPUT,
    AE_WINDOW_CREATED,
    AE_WINDOW_DESTROYED,
    AE_RESUME,
    AE_CLOSE,
} AndroidEventType;

typedef struct {
    AndroidEventType type;
    AInputEvent* inputEvent;
    ANativeWindow* window;
    double time;
} AndroidEvent;

#ifdef __cplusplus
extern "C" {
#endif

/// Calls from TIGR to Android side, render thread
extern int android_pollEvent(int (*eventHandler)(AndroidEvent, void*), void*);
extern void android_swap(EGLDisplay display, EGLSurface surface);
extern void* android_loadAsset(const char* filename, int* outLength);
extern void android_showKeyboard(int show);

/// Calls from Android to TIGR side, main thread
void tigr_android_create();
void tigr_android_destroy();

#ifdef __cplusplus
}
#endif

#endif  // __ANDROID__
#endif  // TIGR_ANDROID_H

//////// End of inlined file: tigr_android.h ////////

//////// Start of inlined file: tigr_linux.c ////////

#ifndef TIGR_HEADLESS

//#include "tigr_internal.h"

#if __linux__ && !__ANDROID__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>

static Display* dpy;
static Window root;
static XVisualInfo* vi;
static Atom wmDeleteMessage;
static XIM inputMethod;
static GLXFBConfig fbConfig;

static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = 0;

static void initX11Stuff() {
    static int done = 0;
    if (!done) {
        dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
            tigrError(0, "Cannot connect to X server");
        }

        root = DefaultRootWindow(dpy);

        static int attribList[] = { GLX_RENDER_TYPE,
                                    GLX_RGBA_BIT,
                                    GLX_DRAWABLE_TYPE,
                                    GLX_WINDOW_BIT,
                                    GLX_DOUBLEBUFFER,
                                    1,
                                    GLX_RED_SIZE,
                                    1,
                                    GLX_GREEN_SIZE,
                                    1,
                                    GLX_BLUE_SIZE,
                                    1,
                                    None };

        int fbcCount = 0;
        GLXFBConfig* fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), attribList, &fbcCount);
        if (!fbc) {
            tigrError(0, "Failed to choose FB config");
        }
        fbConfig = fbc[0];

        vi = glXGetVisualFromFBConfig(dpy, fbConfig);
        if (vi == NULL) {
            tigrError(0, "No appropriate visual found");
        }

        GLXContext tmpCtx = glXCreateContext(dpy, vi, 0, GL_TRUE);
        glXCreateContextAttribsARB =
            (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
        glXDestroyContext(dpy, tmpCtx);
        if (!glXCreateContextAttribsARB) {
            tigrError(0, "Failed to get glXCreateContextAttribsARB");
        }

        inputMethod = XOpenIM(dpy, NULL, NULL, NULL);
        if (inputMethod == NULL) {
            tigrError(0, "Failed to create input method");
        }

        wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

        done = 1;
    }
}

static int hasGLXExtension(Display* display, const char* wanted) {
    const char* extensions = glXQueryExtensionsString(display, DefaultScreen(display));
    char* dup = strdup(extensions);
    char* found = 0;

    for (char* start = dup;; start = 0) {
        found = strtok(start, " ");
        if (found == 0 || strcmp(found, wanted) == 0) {
            break;
        }
    }

    free(dup);
    return found != 0;
}

static void setupVSync(Display* display, Window win) {
    if (hasGLXExtension(display, "GLX_EXT_swap_control")) {
        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT =
            (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");
        if (glXSwapIntervalEXT) {
            glXSwapIntervalEXT(display, win, 1);
        }
    } else if (hasGLXExtension(display, "GLX_MESA_swap_control")) {
        PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA =
            (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalMESA");
        if (glXSwapIntervalMESA) {
            glXSwapIntervalMESA(1);
        }
    } else if (hasGLXExtension(display, "GLX_SGI_swap_control")) {
        PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
            (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalSGI");
        if (glXSwapIntervalSGI) {
            glXSwapIntervalSGI(1);
        }
    }
}

static void tigrHideCursor(TigrInternal* win) {
    Cursor invisibleCursor;
    Pixmap bitmapNoData;
    XColor black;
    static char noData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    black.red = black.green = black.blue = 0;

    bitmapNoData = XCreateBitmapFromData(win->dpy, win->win, noData, 8, 8);
    invisibleCursor = XCreatePixmapCursor(win->dpy, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
    XDefineCursor(win->dpy, win->win, invisibleCursor);
    XFreeCursor(win->dpy, invisibleCursor);
    XFreePixmap(win->dpy, bitmapNoData);
}

typedef struct {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
} WindowHints;

Tigr* tigrWindow(int w, int h, const char* title, int flags) {
    Tigr* bmp = 0;
    Colormap cmap;
    XSetWindowAttributes swa;
    Window xwin;
    GLXContext glc;
    XIC ic;
    int scale;

    initX11Stuff();

    if (flags & TIGR_AUTO) {
        // Always use a 1:1 pixel size, unless downscaled by tigrEnforceScale below.
        scale = 1;
    } else {
        // See how big we can make it and still fit on-screen.
        Screen* screen = DefaultScreenOfDisplay(dpy);
        int maxW = WidthOfScreen(screen);
        int maxH = HeightOfScreen(screen);
        scale = tigrCalcScale(w, h, maxW, maxH);
    }

    scale = tigrEnforceScale(scale, flags);

    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.event_mask = StructureNotifyMask;

    // Create window of wanted size
    xwin = XCreateWindow(dpy, root, 0, 0, w * scale, h * scale, 0, vi->depth, InputOutput, vi->visual,
                         CWColormap | CWEventMask, &swa);
    XMapWindow(dpy, xwin);

    if (flags & TIGR_FULLSCREEN) {
        // https://superuser.com/questions/1680077/does-x11-actually-have-a-native-fullscreen-mode
        Atom wm_state   = XInternAtom (dpy, "_NET_WM_STATE", true );
        Atom wm_fullscreen = XInternAtom (dpy, "_NET_WM_STATE_FULLSCREEN", true );
        XChangeProperty(dpy, xwin, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *)&wm_fullscreen, 1);
    } else {
        // Wait for window to get mapped
        for (;;) {
            XEvent e;
            XNextEvent(dpy, &e);
            if (e.type == MapNotify) {
                break;
            }
        }

        // Reset size if we did not get the window size we wanted above.
        XWindowAttributes wa;
        XGetWindowAttributes(dpy, xwin, &wa);
        scale = tigrCalcScale(w, h, wa.width, wa.height);
        scale = tigrEnforceScale(scale, flags);
        XResizeWindow(dpy, xwin, w * scale, h * scale);
    }

    XTextProperty prop;
    int result = Xutf8TextListToTextProperty(dpy, (char**)&title, 1, XUTF8StringStyle, &prop);
    if (result == Success) {
        Atom wmName = XInternAtom(dpy, "_NET_WM_NAME", 0);
        XSetTextProperty(dpy, xwin, &prop, wmName);
        XFree(prop.value);
    }

    ic = XCreateIC(inputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, xwin, NULL);
    if (ic == NULL) {
        printf("Failed to create input context\n");
        exit(0);
    }
    XSetICFocus(ic);

    XSetWMProtocols(dpy, xwin, &wmDeleteMessage, 1);

    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    int contextAttributes[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3, None };
    glc = glXCreateContextAttribsARB(dpy, fbConfig, NULL, GL_TRUE, contextAttributes);
    glXMakeCurrent(dpy, xwin, glc);

    setupVSync(dpy, xwin);

    bmp = tigrBitmap2(w, h, sizeof(TigrInternal));
    bmp->handle = (void*)xwin;

    TigrInternal* win = tigrInternal(bmp);
    win->win = xwin;
    win->dpy = dpy;
    win->glc = glc;
    win->ic = ic;

    win->shown = 0;
    win->closed = 0;
    win->scale = scale;

    win->lastChar = 0;
    win->flags = flags;
    win->p1 = win->p2 = win->p3 = 0;
    win->p4 = 1;
    win->widgetsWanted = 0;
    win->widgetAlpha = 0;
    win->widgetsScale = 0;
    win->widgets = 0;
    win->gl.gl_legacy = 0;

    memset(win->keys, 0, 256);
    memset(win->prev, 0, 256);

    if (flags & TIGR_NOCURSOR) {
        tigrHideCursor(win);
    }

    tigrPosition(bmp, win->scale, bmp->w, bmp->h, win->pos);
    tigrGAPICreate(bmp);
    tigrGAPIBegin(bmp);

    return bmp;
}

int tigrClosed(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    return win->win == 0;
}

int tigrGAPIBegin(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    return glXMakeCurrent(win->dpy, win->win, win->glc) ? 0 : -1;
}

int tigrGAPIEnd(Tigr* bmp) {
    (void)bmp;
    return glXMakeCurrent(NULL, 0, 0) ? 0 : -1;
}

int tigrKeyDown(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return win->keys[key] && !win->prev[key];
}

int tigrKeyHeld(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return win->keys[key];
}

int tigrReadChar(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    int c = win->lastChar;
    win->lastChar = 0;
    return c;
}

uint8_t tigrKeyFromX11(KeySym sym) {
    if (sym >= 'a' && sym <= 'z') {
        return (uint8_t)sym - ('a' - 'A');
    }

    if (sym >= '0' && sym <= '9') {
        return (uint8_t)sym;
    }

    switch (sym) {
        case XK_KP_0:
            return TK_PAD0;
        case XK_KP_1:
            return TK_PAD1;
        case XK_KP_2:
            return TK_PAD2;
        case XK_KP_3:
            return TK_PAD3;
        case XK_KP_4:
            return TK_PAD4;
        case XK_KP_5:
            return TK_PAD5;
        case XK_KP_6:
            return TK_PAD6;
        case XK_KP_7:
            return TK_PAD7;
        case XK_KP_8:
            return TK_PAD8;
        case XK_KP_9:
            return TK_PAD9;

        case XK_KP_Multiply:
            return TK_PADMUL;
        case XK_KP_Divide:
            return TK_PADDIV;
        case XK_KP_Add:
            return TK_PADADD;
        case XK_KP_Subtract:
            return TK_PADSUB;
        case XK_KP_Decimal:
            return TK_PADDOT;
        case XK_KP_Enter:
            return TK_PADENTER;

        case XK_F1:
            return TK_F1;
        case XK_F2:
            return TK_F2;
        case XK_F3:
            return TK_F3;
        case XK_F4:
            return TK_F4;
        case XK_F5:
            return TK_F5;
        case XK_F6:
            return TK_F6;
        case XK_F7:
            return TK_F7;
        case XK_F8:
            return TK_F8;
        case XK_F9:
            return TK_F9;
        case XK_F10:
            return TK_F10;
        case XK_F11:
            return TK_F11;
        case XK_F12:
            return TK_F12;

        case XK_BackSpace:
            return TK_BACKSPACE;
        case XK_Tab:
            return TK_TAB;
        case XK_Return:
            return TK_RETURN;
        case XK_Pause:
            return TK_PAUSE;
        case XK_Caps_Lock:
            return TK_CAPSLOCK;
        case XK_Escape:
            return TK_ESCAPE;
        case XK_space:
            return TK_SPACE;

        case XK_Page_Up:
            return TK_PAGEUP;
        case XK_Page_Down:
            return TK_PAGEDN;
        case XK_End:
            return TK_END;
        case XK_Home:
            return TK_HOME;
        case XK_Left:
            return TK_LEFT;
        case XK_Up:
            return TK_UP;
        case XK_Right:
            return TK_RIGHT;
        case XK_Down:
            return TK_DOWN;
        case XK_Insert:
            return TK_INSERT;
        case XK_Delete:
            return TK_DELETE;

        case XK_Meta_L:
            return TK_LWIN;
        case XK_Meta_R:
            return TK_RWIN;
        case XK_Num_Lock:
            return TK_NUMLOCK;
        case XK_Scroll_Lock:
            return TK_SCROLL;
        case XK_Shift_L:
            return TK_LSHIFT;
        case XK_Shift_R:
            return TK_RSHIFT;
        case XK_Control_L:
            return TK_LCONTROL;
        case XK_Control_R:
            return TK_RCONTROL;
        case XK_Alt_L:
            return TK_LALT;
        case XK_Alt_R:
            return TK_RALT;

        case XK_semicolon:
            return TK_SEMICOLON;
        case XK_equal:
            return TK_EQUALS;
        case XK_comma:
            return TK_COMMA;
        case XK_minus:
            return TK_MINUS;
        case XK_period:
            return TK_DOT;
        case XK_slash:
            return TK_SLASH;
        case XK_grave:
            return TK_BACKTICK;
        case XK_bracketleft:
            return TK_LSQUARE;
        case XK_backslash:
            return TK_BACKSLASH;
        case XK_bracketright:
            return TK_RSQUARE;
        case XK_apostrophe:
            return TK_TICK;
    }
    return 0;
}

static void tigrUpdateModifiers(TigrInternal* win) {
    win->keys[TK_SHIFT] = win->keys[TK_LSHIFT] || win->keys[TK_RSHIFT];
    win->keys[TK_CONTROL] = win->keys[TK_LCONTROL] || win->keys[TK_RCONTROL];
    win->keys[TK_ALT] = win->keys[TK_LALT] || win->keys[TK_RALT];
}

static void tigrInterpretChar(TigrInternal* win, Window root, unsigned int keycode, unsigned int mask) {
    XKeyEvent event;
    memset(&event, 0, sizeof(event));
    event.type = KeyPress;
    event.display = win->dpy;
    event.root = root;
    event.window = win->win;
    event.state = mask;
    event.keycode = keycode;
    char inputTextUTF8[10];
    Status status = 0;
    Xutf8LookupString(win->ic, &event, inputTextUTF8, sizeof(inputTextUTF8), NULL, &status);

    if (status == XLookupChars) {
        tigrDecodeUTF8(inputTextUTF8, &win->lastChar);
    }
}

static void tigrProcessInput(TigrInternal* win, int winWidth, int winHeight) {
    {
        Window focused;
        int revertTo;
        XGetInputFocus(win->dpy, &focused, &revertTo);

        if (win->win != focused) {
            return;
        }
    }

    Window root;
    Window child;
    int rootX;
    int rootY;
    int winX;
    int winY;
    unsigned int mask;

    if (XQueryPointer(win->dpy, win->win, &root, &child, &rootX, &rootY, &winX, &winY, &mask)) {
        static unsigned int prevButtons;
        unsigned int buttons = mask & (Button1Mask | Button2Mask | Button3Mask);

        win->mouseX = (winX - win->pos[0]) / win->scale;
        win->mouseY = (winY - win->pos[1]) / win->scale;

        if (buttons != prevButtons && (winX > 0 && winX < winWidth) && (winY > 0 && winY < winHeight)) {
            win->mouseButtons = (buttons & Button1Mask)       ? 1
                                : 0 | (buttons & Button3Mask) ? 2
                                : 0 | (buttons & Button2Mask) ? 4
                                                              : 0;
        }
        prevButtons = buttons;
    }

    static char prevKeys[32];
    char keys[32];
    XQueryKeymap(win->dpy, keys);
    for (int i = 0; i < 32; i++) {
        char thisBlock = keys[i];
        char prevBlock = prevKeys[i];
        if (thisBlock != prevBlock) {
            for (int j = 0; j < 8; j++) {
                int thisBit = thisBlock & 1;
                int prevBit = prevBlock & 1;
                thisBlock >>= 1;
                prevBlock >>= 1;
                if (thisBit != prevBit) {
                    int keyCode = 8 * i + j;
                    KeySym keySym = XkbKeycodeToKeysym(win->dpy, keyCode, 0, 0);
                    if (keySym != NoSymbol) {
                        int key = tigrKeyFromX11(keySym);
                        win->keys[key] = thisBit;
                        tigrUpdateModifiers(win);

                        if (thisBit) {
                            tigrInterpretChar(win, root, keyCode, mask);
                        }
                    }
                }
            }
        }
    }
    memcpy(prevKeys, keys, 32);

    XEvent event;
    while (XCheckTypedWindowEvent(win->dpy, win->win, ClientMessage, &event)) {
        if (event.xclient.data.l[0] == wmDeleteMessage) {
            glXMakeCurrent(win->dpy, None, NULL);
            glXDestroyContext(win->dpy, win->glc);
            XDestroyWindow(win->dpy, win->win);
            win->win = 0;
        }
    }
    XFlush(win->dpy);
}

void tigrUpdate(Tigr* bmp) {
    XWindowAttributes gwa;

    TigrInternal* win = tigrInternal(bmp);

    memcpy(win->prev, win->keys, 256);

    XGetWindowAttributes(win->dpy, win->win, &gwa);

    if (win->flags & TIGR_AUTO)
        tigrResize(bmp, gwa.width / win->scale, gwa.height / win->scale);
    else
        win->scale = tigrEnforceScale(tigrCalcScale(bmp->w, bmp->h, gwa.width, gwa.height), win->flags);

    tigrPosition(bmp, win->scale, gwa.width, gwa.height, win->pos);
    glXMakeCurrent(win->dpy, win->win, win->glc);
    tigrGAPIPresent(bmp, gwa.width, gwa.height);
    glXSwapBuffers(win->dpy, win->win);

    tigrProcessInput(win, gwa.width, gwa.height);
}

void tigrFree(Tigr* bmp) {
    if (bmp->handle) {
        TigrInternal* win = tigrInternal(bmp);
        if (win->win) {
            glXMakeCurrent(win->dpy, None, NULL);
            glXDestroyContext(win->dpy, win->glc);
            XDestroyWindow(win->dpy, win->win);
            win->win = 0;
        }
    }
    free(bmp->pix);
    free(bmp);
}

void tigrError(Tigr* bmp, const char* message, ...) {
    char tmp[1024];

    va_list args;
    va_start(args, message);
    vsnprintf(tmp, sizeof(tmp), message, args);
    tmp[sizeof(tmp) - 1] = 0;
    va_end(args);

    printf("tigr fatal error: %s\n", tmp);

    exit(1);
}

float tigrTime() {
    static double lastTime = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);

    double now = (double)tv.tv_sec + (tv.tv_usec / 1000000.0);
    double elapsed = lastTime == 0 ? 0 : now - lastTime;
    lastTime = now;

    return (float)elapsed;
}

void tigrMouse(Tigr* bmp, int* x, int* y, int* buttons) {
    TigrInternal* win = tigrInternal(bmp);
    if (x) {
        *x = win->mouseX;
    }
    if (y) {
        *y = win->mouseY;
    }
    if (buttons) {
        *buttons = win->mouseButtons;
    }
}

int tigrTouch(Tigr* bmp, TigrTouchPoint* points, int maxPoints) {
    int buttons = 0;
    if (maxPoints > 0) {
        tigrMouse(bmp, &points->x, &points->y, &buttons);
    }
    return buttons ? 1 : 0;
}

#endif  // __linux__ && !__ANDROID__

#endif // #ifndef TIGR_HEADLESS

//////// End of inlined file: tigr_linux.c ////////

//////// Start of inlined file: tigr_android.c ////////

#ifndef TIGR_HEADLESS

//#include "tigr_internal.h"

#ifdef __ANDROID__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <android/log.h>
#include <android/native_window.h>
#include <android/input.h>

#ifndef NDEBUG
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "tigr", __VA_ARGS__))
#else
#define LOGD(...) ((void)0)
#endif
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "tigr", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "tigr", __VA_ARGS__))

typedef struct {
    TigrTouchPoint points[MAX_TOUCH_POINTS];
    int numPoints;
} InputState;

/// Global state
static struct {
    ANativeWindow* window;
    InputState inputState;
    EGLDisplay display;
    EGLSurface surface;
    EGLint screenW;
    EGLint screenH;
    EGLConfig config;
    double lastTime;
    int closed;
} gState = {
    .window = 0,
    .inputState = {
        .numPoints = 0,
    },
    .display = EGL_NO_DISPLAY,
    .surface = EGL_NO_SURFACE,
    .screenW = 0,
    .screenH = 0,
    .config = 0,
    .lastTime = 0,
    .closed = 0,
};

static const EGLint contextAttribs[] = { EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 0, EGL_NONE };

static EGLConfig getGLConfig(EGLDisplay display) {
    EGLConfig config = 0;

    const EGLint attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
                               EGL_NONE };
    EGLint numConfigs;

    eglChooseConfig(display, attribs, NULL, 0, &numConfigs);
    EGLConfig* supportedConfigs = (EGLConfig*)malloc(sizeof(EGLConfig) * numConfigs);
    eglChooseConfig(display, attribs, supportedConfigs, numConfigs, &numConfigs);

    int i = 0;
    for (; i < numConfigs; i++) {
        EGLConfig* cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) && r == 8 && g == 8 && b == 8 && d == 0) {
            config = supportedConfigs[i];
            break;
        }
    }

    if (i == numConfigs) {
        config = supportedConfigs[0];
    }

    if (config == NULL) {
        tigrError(NULL, "Unable to initialize EGLConfig");
    }

    free(supportedConfigs);

    return config;
}

/// Android interface, called from main thread

void tigr_android_create() {
    gState.closed = 0;
    gState.window = 0;
    gState.inputState.numPoints = 0;
    gState.surface = EGL_NO_SURFACE;
    gState.lastTime = 0;

    if (gState.display == EGL_NO_DISPLAY) {
        gState.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        EGLBoolean status = eglInitialize(gState.display, NULL, NULL);
        if (!status) {
            tigrError(NULL, "Failed to init EGL");
        }
        gState.config = getGLConfig(gState.display);
    }
}

void tigr_android_destroy() {
    eglTerminate(gState.display);
    gState.display = EGL_NO_DISPLAY;
}

/// Internals ///

static void logEglError() {
    int error = eglGetError();
    switch (error) {
        case EGL_BAD_DISPLAY:
            LOGE("EGL error: Bad display");
            break;
        case EGL_NOT_INITIALIZED:
            LOGE("EGL error: Not initialized");
            break;
        case EGL_BAD_NATIVE_WINDOW:
            LOGE("EGL error: Bad native window");
            break;
        case EGL_BAD_ALLOC:
            LOGE("EGL error: Bad alloc");
            break;
        case EGL_BAD_MATCH:
            LOGE("EGL error: Bad match");
            break;
        default:
            LOGE("EGL error: %d", error);
    }
}

static void setupOpenGL() {
    LOGD("setupOpenGL");
    assert(gState.surface == EGL_NO_SURFACE);
    assert(gState.window != 0);

    gState.surface = eglCreateWindowSurface(gState.display, gState.config, gState.window, NULL);
    if (gState.surface == EGL_NO_SURFACE) {
        logEglError();
    }
    assert(gState.surface != EGL_NO_SURFACE);
    eglQuerySurface(gState.display, gState.surface, EGL_WIDTH, &gState.screenW);
    eglQuerySurface(gState.display, gState.surface, EGL_HEIGHT, &gState.screenH);
    LOGD("Screen is %d x %d", gState.screenW, gState.screenH);
}

static void tearDownOpenGL() {
    LOGD("tearDownOpenGL");
    if (gState.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(gState.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (gState.surface != EGL_NO_SURFACE) {
            LOGD("eglDestroySurface");
            if (!eglDestroySurface(gState.display, gState.surface)) {
                logEglError();
            }
            gState.surface = EGL_NO_SURFACE;
        }
    }
}

static int tigrKeyFromAndroidKey(int key) {
    switch (key) {
        case AKEYCODE_Q:
            return 'Q';
        case AKEYCODE_W:
            return 'W';
        case AKEYCODE_E:
            return 'E';
        case AKEYCODE_R:
            return 'R';
        case AKEYCODE_T:
            return 'T';
        case AKEYCODE_Y:
            return 'Y';
        case AKEYCODE_U:
            return 'U';
        case AKEYCODE_I:
            return 'I';
        case AKEYCODE_O:
            return 'O';
        case AKEYCODE_P:
            return 'P';

        case AKEYCODE_A:
            return 'A';
        case AKEYCODE_S:
            return 'S';
        case AKEYCODE_D:
            return 'D';
        case AKEYCODE_F:
            return 'F';
        case AKEYCODE_G:
            return 'G';
        case AKEYCODE_H:
            return 'H';
        case AKEYCODE_J:
            return 'J';
        case AKEYCODE_K:
            return 'K';
        case AKEYCODE_L:
            return 'L';

        case AKEYCODE_Z:
            return 'Z';
        case AKEYCODE_X:
            return 'X';
        case AKEYCODE_C:
            return 'C';
        case AKEYCODE_V:
            return 'V';
        case AKEYCODE_B:
            return 'B';
        case AKEYCODE_N:
            return 'N';
        case AKEYCODE_M:
            return 'M';

        case AKEYCODE_0:
            return '0';
        case AKEYCODE_1:
            return '1';
        case AKEYCODE_2:
            return '2';
        case AKEYCODE_3:
            return '3';
        case AKEYCODE_4:
            return '4';
        case AKEYCODE_5:
            return '5';
        case AKEYCODE_6:
            return '6';
        case AKEYCODE_7:
            return '7';
        case AKEYCODE_8:
            return '8';
        case AKEYCODE_9:
            return '9';

        case AKEYCODE_NUMPAD_0:
            return TK_PAD0;
        case AKEYCODE_NUMPAD_1:
            return TK_PAD1;
        case AKEYCODE_NUMPAD_2:
            return TK_PAD2;
        case AKEYCODE_NUMPAD_3:
            return TK_PAD3;
        case AKEYCODE_NUMPAD_4:
            return TK_PAD4;
        case AKEYCODE_NUMPAD_5:
            return TK_PAD5;
        case AKEYCODE_NUMPAD_6:
            return TK_PAD6;
        case AKEYCODE_NUMPAD_7:
            return TK_PAD7;
        case AKEYCODE_NUMPAD_8:
            return TK_PAD8;
        case AKEYCODE_NUMPAD_9:
            return TK_PAD9;

        case AKEYCODE_NUMPAD_MULTIPLY:
            return TK_PADMUL;
        case AKEYCODE_NUMPAD_DIVIDE:
            return TK_PADDIV;
        case AKEYCODE_NUMPAD_ADD:
            return TK_PADADD;
        case AKEYCODE_NUMPAD_SUBTRACT:
            return TK_PADSUB;
        case AKEYCODE_NUMPAD_ENTER:
            return TK_PADENTER;
        case AKEYCODE_NUMPAD_DOT:
            return TK_PADDOT;

        case AKEYCODE_F1:
            return TK_F1;
        case AKEYCODE_F2:
            return TK_F2;
        case AKEYCODE_F3:
            return TK_F3;
        case AKEYCODE_F4:
            return TK_F4;
        case AKEYCODE_F5:
            return TK_F5;
        case AKEYCODE_F6:
            return TK_F6;
        case AKEYCODE_F7:
            return TK_F7;
        case AKEYCODE_F8:
            return TK_F8;
        case AKEYCODE_F9:
            return TK_F9;
        case AKEYCODE_F10:
            return TK_F10;
        case AKEYCODE_F11:
            return TK_F11;
        case AKEYCODE_F12:
            return TK_F12;

        case AKEYCODE_SHIFT_LEFT:
            return TK_LSHIFT;
        case AKEYCODE_SHIFT_RIGHT:
            return TK_RSHIFT;
        case AKEYCODE_CTRL_LEFT:
            return TK_LCONTROL;
        case AKEYCODE_CTRL_RIGHT:
            return TK_RCONTROL;
        case AKEYCODE_ALT_LEFT:
            return TK_LALT;
        case AKEYCODE_ALT_RIGHT:
            return TK_RALT;
        case AKEYCODE_META_LEFT:
            return TK_LWIN;
        case AKEYCODE_META_RIGHT:
            return TK_RWIN;

        case AKEYCODE_DEL:
            return TK_BACKSPACE;
        case AKEYCODE_TAB:
            return TK_TAB;
        case AKEYCODE_ENTER:
            return TK_RETURN;
        case AKEYCODE_CAPS_LOCK:
            return TK_CAPSLOCK;
        case AKEYCODE_ESCAPE:
        case AKEYCODE_BACK:
            return TK_ESCAPE;
        case AKEYCODE_SPACE:
            return TK_SPACE;

        case AKEYCODE_PAGE_UP:
            return TK_PAGEUP;
        case AKEYCODE_PAGE_DOWN:
            return TK_PAGEDN;
        case AKEYCODE_MOVE_END:
            return TK_END;
        case AKEYCODE_MOVE_HOME:
            return TK_HOME;
        case AKEYCODE_DPAD_LEFT:
            return TK_LEFT;
        case AKEYCODE_DPAD_RIGHT:
            return TK_RIGHT;
        case AKEYCODE_DPAD_UP:
            return TK_UP;
        case AKEYCODE_DPAD_DOWN:
            return TK_DOWN;

        case AKEYCODE_INSERT:
            return TK_INSERT;
        case AKEYCODE_FORWARD_DEL:
            return TK_DELETE;
        case AKEYCODE_NUM_LOCK:
            return TK_NUMLOCK;
        case AKEYCODE_SCROLL_LOCK:
            return TK_SCROLL;

        case AKEYCODE_SEMICOLON:
            return TK_SEMICOLON;
        case AKEYCODE_EQUALS:
            return TK_EQUALS;
        case AKEYCODE_COMMA:
            return TK_COMMA;
        case AKEYCODE_MINUS:
            return TK_MINUS;
        case AKEYCODE_PERIOD:
            return TK_DOT;
        case AKEYCODE_SLASH:
            return TK_SLASH;
        case AKEYCODE_BACKSLASH:
            return TK_BACKSLASH;
        case AKEYCODE_GRAVE:
            return TK_BACKTICK;
        case AKEYCODE_APOSTROPHE:
            return TK_TICK;
        case AKEYCODE_LEFT_BRACKET:
            return TK_LSQUARE;
        case AKEYCODE_RIGHT_BRACKET:
            return TK_RSQUARE;

        default:
            return 0;
    }
}

static int processInputEvent(AInputEvent* event, TigrInternal* win) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        int32_t actionCode = action & AMOTION_EVENT_ACTION_MASK;

        size_t touchPoints = AMotionEvent_getPointerCount(event);
        size_t releasedIndex = -1;
        if (actionCode == AMOTION_EVENT_ACTION_POINTER_UP) {
            releasedIndex =
                (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        }
        size_t targetPointCount = 0;

        for (size_t i = 0; i < touchPoints && i < MAX_TOUCH_POINTS; i++) {
            if (i == releasedIndex) {
                continue;
            }
            gState.inputState.points[targetPointCount].x = AMotionEvent_getX(event, i);
            gState.inputState.points[targetPointCount].y = AMotionEvent_getY(event, i);
            targetPointCount++;
        }

        if (actionCode == AMOTION_EVENT_ACTION_UP || actionCode == AMOTION_EVENT_ACTION_CANCEL) {
            gState.inputState.numPoints = 0;
        } else {
            gState.inputState.numPoints = targetPointCount;
        }
        return 1;
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        if (!win) {
            return 1;
        }
        int32_t deviceID = AInputEvent_getDeviceId(event);
        // KeyCharacterMap#VIRTUAL_KEYBOARD == -1
        if (deviceID != -1) {
            return 1;
        }
        int32_t action = AKeyEvent_getAction(event);
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        int key = tigrKeyFromAndroidKey(keyCode);
        // We pass the character in the scancode field from the Java side
        int32_t unicodeChar = AKeyEvent_getScanCode(event);

        if (action == AKEY_EVENT_ACTION_DOWN) {
            win->keys[key] = 1;
            win->lastChar = unicodeChar;
        } else if (action == AKEY_EVENT_ACTION_UP) {
            win->released[key] = 1;
        }
        return 1;
    }
    return 0;
}

static int handleEvent(AndroidEvent event, void* userData) {
    switch (event.type) {
        case AE_WINDOW_CREATED:
            gState.window = event.window;
            setupOpenGL();
            break;

        case AE_WINDOW_DESTROYED:
            tearDownOpenGL();
            gState.window = 0;
            break;

        case AE_INPUT:
            return processInputEvent(event.inputEvent, (TigrInternal*)userData);

        case AE_RESUME:
            gState.lastTime = event.time;
            break;

        case AE_CLOSE:
            gState.closed = 1;
            break;

        default:
            LOGE("Unhandled event type: %d", event.type);
            return 0;
    }

    return 1;
}

static int processEvents(TigrInternal* win) {
    if (gState.closed) {
        return 0;
    }

    while (android_pollEvent(handleEvent, win)) {
        if (gState.closed) {
            return 0;
        }
    }

    return 1;
}

static Tigr* refreshWindow(Tigr* bmp) {
    if (bmp->handle == gState.window) {
        return bmp;
    }

    bmp->handle = gState.window;
    if (gState.window == 0) {
        return 0;
    }

    TigrInternal* win = tigrInternal(bmp);

    int scale = 1;
    if (win->flags & TIGR_AUTO) {
        // Always use a 1:1 pixel size.
        scale = 1;
    } else {
        // See how big we can make it and still fit on-screen.
        scale = tigrCalcScale(bmp->w, bmp->h, gState.screenW, gState.screenH);
    }

    win->scale = tigrEnforceScale(scale, win->flags);

    return bmp;
}

/// TIGR interface implementation, called from render thread ///

Tigr* tigrWindow(int w, int h, const char* title, int flags) {
    while (gState.window == NULL) {
        if (!processEvents(0)) {
            return NULL;
        }
    }

    EGLContext context = eglCreateContext(gState.display, gState.config, NULL, contextAttribs);

    int scale = 1;
    if (flags & TIGR_AUTO) {
        // Always use a 1:1 pixel size.
        scale = 1;
    } else {
        // See how big we can make it and still fit on-screen.
        scale = tigrCalcScale(w, h, gState.screenW, gState.screenH);
    }

    scale = tigrEnforceScale(scale, flags);

    Tigr* bmp = tigrBitmap2(w, h, sizeof(TigrInternal));
    bmp->handle = (void*)gState.window;

    TigrInternal* win = tigrInternal(bmp);
    win->context = context;

    win->shown = 0;
    win->closed = 0;
    win->scale = scale;

    win->lastChar = 0;
    win->flags = flags;
    win->p1 = win->p2 = win->p3 = 0;
    win->p4 = 1;
    win->widgetsWanted = 0;
    win->widgetAlpha = 0;
    win->widgetsScale = 0;
    win->widgets = 0;
    win->gl.gl_legacy = 0;

    memset(win->keys, 0, 256);
    memset(win->prev, 0, 256);
    memset(win->released, 0, 256);

    tigrPosition(bmp, win->scale, bmp->w, bmp->h, win->pos);

    if (eglMakeCurrent(gState.display, gState.surface, gState.surface, context) == EGL_FALSE) {
        LOGE("Unable to eglMakeCurrent");
        return 0;
    }

    tigrGAPICreate(bmp);

    return bmp;
}

int tigrClosed(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    return win->closed;
}

int tigrGAPIBegin(Tigr* bmp) {
    assert(gState.display != EGL_NO_DISPLAY);
    assert(gState.surface != EGL_NO_SURFACE);

    TigrInternal* win = tigrInternal(bmp);
    if (eglMakeCurrent(gState.display, gState.surface, gState.surface, win->context) == EGL_FALSE) {
        return -1;
    }
    return 0;
}

int tigrGAPIEnd(Tigr* bmp) {
    (void)bmp;
    eglMakeCurrent(gState.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    return 0;
}

int tigrKeyDown(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return win->keys[key] && !win->prev[key];
}

int tigrKeyHeld(Tigr* bmp, int key) {
    TigrInternal* win;
    assert(key < 256);
    win = tigrInternal(bmp);
    return win->keys[key];
}

int tigrReadChar(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    int c = win->lastChar;
    win->lastChar = 0;
    return c;
}

static void tigrUpdateModifiers(TigrInternal* win) {
    win->keys[TK_SHIFT] = win->keys[TK_LSHIFT] || win->keys[TK_RSHIFT];
    win->keys[TK_CONTROL] = win->keys[TK_LCONTROL] || win->keys[TK_RCONTROL];
    win->keys[TK_ALT] = win->keys[TK_LALT] || win->keys[TK_RALT];
}

static int toWindowX(TigrInternal* win, int x) {
    return (x - win->pos[0]) / win->scale;
}

static int toWindowY(TigrInternal* win, int y) {
    return (y - win->pos[1]) / win->scale;
}

void tigrUpdate(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    memcpy(win->prev, win->keys, 256);
    for (int i = 0; i < 256; i++) {
        win->keys[i] ^= win->released[i];
        win->released[i] = 0;
    }

    if (!processEvents(win)) {
        win->closed = 1;
        return;
    }

    tigrUpdateModifiers(win);

    if (gState.window == 0) {
        return;
    }

    bmp = refreshWindow(bmp);
    if (bmp == 0) {
        return;
    }

    win->numTouchPoints = gState.inputState.numPoints;
    for (int i = 0; i < win->numTouchPoints; i++) {
        win->touchPoints[i].x = toWindowX(win, gState.inputState.points[i].x);
        win->touchPoints[i].y = toWindowY(win, gState.inputState.points[i].y);
    }

    win->mouseButtons = win->numTouchPoints;
    if (win->mouseButtons > 0) {
        win->mouseX = win->touchPoints[0].x;
        win->mouseY = win->touchPoints[0].y;
    }

    if (win->flags & TIGR_AUTO) {
        tigrResize(bmp, gState.screenW / win->scale, gState.screenH / win->scale);
    } else {
        win->scale = tigrEnforceScale(tigrCalcScale(bmp->w, bmp->h, gState.screenW, gState.screenH), win->flags);
    }

    tigrPosition(bmp, win->scale, gState.screenW, gState.screenH, win->pos);
    tigrGAPIBegin(bmp);
    tigrGAPIPresent(bmp, gState.screenW, gState.screenH);
    android_swap(gState.display, gState.surface);
    tigrGAPIEnd(bmp);
}

void tigrFree(Tigr* bmp) {
    if (bmp->handle) {
        TigrInternal* win = tigrInternal(bmp);

        eglMakeCurrent(gState.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (win->context != EGL_NO_CONTEXT) {
            // Win closed means app windows has closed, and the call would fail.
            if (!win->closed) {
                tigrGAPIDestroy(bmp);
            }
            eglDestroyContext(gState.display, win->context);
        }

        win->context = EGL_NO_CONTEXT;
    }
    free(bmp->pix);
    free(bmp);
}

void tigrError(Tigr* bmp, const char* message, ...) {
    char tmp[1024];

    va_list args;
    va_start(args, message);
    vsnprintf(tmp, sizeof(tmp), message, args);
    tmp[sizeof(tmp) - 1] = 0;
    va_end(args);

    LOGE("tigr fatal error: %s\n", tmp);

    exit(1);
}

float tigrTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    double now = (double)ts.tv_sec + (ts.tv_nsec / 1000000000.0);
    double elapsed = gState.lastTime == 0 ? 0 : now - gState.lastTime;
    gState.lastTime = now;

    return (float)elapsed;
}

void tigrMouse(Tigr* bmp, int* x, int* y, int* buttons) {
    TigrInternal* win = tigrInternal(bmp);
    if (x) {
        *x = win->mouseX;
    }
    if (y) {
        *y = win->mouseY;
    }
    if (buttons) {
        *buttons = win->mouseButtons;
    }
}

int tigrTouch(Tigr* bmp, TigrTouchPoint* points, int maxPoints) {
    TigrInternal* win = tigrInternal(bmp);
    for (int i = 0; i < maxPoints && i < win->numTouchPoints; i++) {
        points[i] = win->touchPoints[i];
    }
    return maxPoints < win->numTouchPoints ? maxPoints : win->numTouchPoints;
}

void* tigrReadFile(const char* fileName, int* length) {
    if (length != 0) {
        *length = 0;
    }

    void* asset = android_loadAsset(fileName, length);
    return asset;
}

#endif  // __ANDROID__
#endif // #ifndef TIGR_HEADLESS

//////// End of inlined file: tigr_android.c ////////

//////// Start of inlined file: tigr_gl.c ////////

#ifndef TIGR_HEADLESS

//#include "tigr_internal.h"
#include <assert.h>

#ifdef TIGR_GAPI_GL
#if __linux__
#if __ANDROID__
#include <GLES3/gl3.h>
#else
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glext.h>
#endif
#endif
extern const char tigr_upscale_gl_vs[], tigr_upscale_gl_fs[], tigr_default_fx_gl_fs[];
extern const int tigr_upscale_gl_vs_size, tigr_upscale_gl_fs_size, tigr_default_fx_gl_fs_size;

#ifdef _WIN32

#ifdef TIGR_GAPI_GL_WIN_USE_GLEXT
#include <glext.h>
#include <wglext.h>
#else  // short version of glext.h and wglext.h so we don't need to depend on them
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY*
#endif
typedef ptrdiff_t GLsizeiptr;
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_BGRA 0x80E1
#define GL_TEXTURE0 0x84C0
typedef void(APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void(APIENTRYP PFNGLGENBUFFERSARBPROC)(GLsizei n, GLuint* buffers);
typedef void(APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void(APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void(APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void(APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void(APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index,
                                                     GLint size,
                                                     GLenum type,
                                                     GLboolean normalized,
                                                     GLsizei stride,
                                                     const void* pointer);
typedef GLuint(APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
typedef char GLchar;
typedef void(APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader,
                                              GLsizei count,
                                              const GLchar* const* string,
                                              const GLint* length);
typedef void(APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint(APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
typedef void(APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void(APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void(APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
typedef void(APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void(APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void(APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void(APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void(APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void(APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef GLint(APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
typedef void(APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void(APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location,
                                                  GLsizei count,
                                                  GLboolean transpose,
                                                  const GLfloat* value);
typedef void(APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC hdc,
                                                     const int* piAttribIList,
                                                     const FLOAT* pfAttribFList,
                                                     UINT nMaxFormats,
                                                     int* piFormats,
                                                     UINT* nNumFormats);
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);
#endif

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormat;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGENBUFFERSARBPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLACTIVETEXTUREPROC glActiveTexture;
int tigrGL11Init(Tigr* bmp) {
    int pixel_format;
    TigrInternal* win = tigrInternal(bmp);
    GLStuff* gl = &win->gl;
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),
                                  1,
                                  PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SWAP_EXCHANGE,
                                  PFD_TYPE_RGBA,
                                  32,  // color bits
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  24,  // depth
                                  8,   // stencil
                                  0,
                                  PFD_MAIN_PLANE,  // is it ignored ?
                                  0,
                                  0,
                                  0,
                                  0 };
    if (!(gl->dc = GetDC((HWND)bmp->handle))) {
        tigrError(bmp, "Cannot create OpenGL device context.\n");
        return -1;
    }
    if (!(pixel_format = ChoosePixelFormat(gl->dc, &pfd))) {
        tigrError(bmp, "Cannot choose OpenGL pixel format.\n");
        return -1;
    }
    if (!SetPixelFormat(gl->dc, pixel_format, &pfd)) {
        tigrError(bmp, "Cannot set OpenGL pixel format.\n");
        return -1;
    }
    if (!(gl->hglrc = wglCreateContext(gl->dc))) {
        tigrError(bmp, "Cannot create OpenGL context.\n");
        return -1;
    }
    if (!wglMakeCurrent(gl->dc, gl->hglrc)) {
        tigrError(bmp, "Cannot activate OpenGL context.\n");
        return -1;
    }
    gl->gl_legacy = 1;
    return 0;
}
int tigrGL33Init(Tigr* bmp) {
    int pixel_format;
    UINT num_formats;
    TigrInternal* win = tigrInternal(bmp);
    GLStuff* gl = &win->gl;

    wglChoosePixelFormat = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    wglCreateContextAttribs = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glGenBuffers = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

    if (!wglChoosePixelFormat || !wglCreateContextAttribs) {
        tigrError(bmp, "Cannot create OpenGL context.\n");
        return -1;
    }
    const int attribList[] = { WGL_DRAW_TO_WINDOW_ARB,
                               GL_TRUE,
                               WGL_SUPPORT_OPENGL_ARB,
                               GL_TRUE,
                               WGL_DOUBLE_BUFFER_ARB,
                               GL_TRUE,
                               WGL_PIXEL_TYPE_ARB,
                               WGL_TYPE_RGBA_ARB,
                               WGL_COLOR_BITS_ARB,
                               32,
                               WGL_DEPTH_BITS_ARB,
                               24,
                               WGL_STENCIL_BITS_ARB,
                               8,
                               0 };
    int attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 3, WGL_CONTEXT_MINOR_VERSION_ARB, 3, 0 };
    if (!wglChoosePixelFormat(gl->dc, attribList, NULL, 1, &pixel_format, &num_formats)) {
        tigrError(bmp, "Cannot choose OpenGL pixel format.\n");
        return -1;
    }
    if (!(gl->hglrc = wglCreateContextAttribs(gl->dc, gl->hglrc, attribs))) {
        tigrError(bmp, "Cannot create OpenGL context attribs.\n");
        return -1;
    }
    if (!wglMakeCurrent(gl->dc, gl->hglrc)) {
        tigrError(bmp, "Cannot activate OpenGL context.\n");
        return -1;
    }
    gl->gl_legacy = 0;
    return 0;
}
#endif

void tigrCheckGLError(const char* state) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        tigrError(NULL, "got GL error %x when doing %s\n", err, state);
    }
}

void tigrCheckShaderErrors(GLuint object) {
    GLint success;
    GLchar info[2048];
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(object, sizeof(info), NULL, info);
        tigrError(NULL, "shader compile error : %s\n", info);
    }
}

void tigrCheckProgramErrors(GLuint object) {
    GLint success;
    GLchar info[2048];
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(object, sizeof(info), NULL, info);
        tigrError(NULL, "shader link error : %s\n", info);
    }
}

void tigrCreateShaderProgram(GLStuff* gl, const char* fxSource, int fxSize) {
    if (gl->program != 0) {
        glDeleteProgram(gl->program);
        gl->program = 0;
    }

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vs_source = (const char*)&tigr_upscale_gl_vs;
    glShaderSource(vs, 1, &vs_source, &tigr_upscale_gl_vs_size);
    glCompileShader(vs);
    tigrCheckShaderErrors(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fs_sources[] = {
        (const char*)tigr_upscale_gl_fs,
        fxSource,
    };
    const int fs_lengths[] = {
        tigr_upscale_gl_fs_size,
        fxSize,
    };
    glShaderSource(fs, 2, fs_sources, fs_lengths);
    glCompileShader(fs);
    tigrCheckShaderErrors(fs);

    gl->program = glCreateProgram();
    glAttachShader(gl->program, vs);
    glAttachShader(gl->program, fs);
    glLinkProgram(gl->program);
    tigrCheckProgramErrors(gl->program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    gl->uniform_projection = glGetUniformLocation(gl->program, "projection");
    gl->uniform_model = glGetUniformLocation(gl->program, "model");
    gl->uniform_parameters = glGetUniformLocation(gl->program, "parameters");
}

void tigrGAPICreate(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    GLStuff* gl = &win->gl;
    GLuint VBO;
    GLfloat vertices[] = { // pos      uv
                           0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };

#ifdef _WIN32
    if (tigrGL11Init(bmp))
        return;
    tigrGL33Init(bmp);
#endif

    if (!gl->gl_legacy) {
        // create vao
        glGenVertexArrays(1, &gl->vao);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(gl->vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

        // create program
        tigrCreateShaderProgram(gl, tigr_default_fx_gl_fs, tigr_default_fx_gl_fs_size);
    }

    // create textures
    if (gl->gl_legacy) {
        glEnable(GL_TEXTURE_2D);
    }
    glGenTextures(2, gl->tex);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, gl->tex[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl->gl_legacy ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl->gl_legacy ? GL_NEAREST : GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    tigrCheckGLError("initialization");
}

void tigrGAPIDestroy(Tigr* bmp) {
    TigrInternal* win = tigrInternal(bmp);
    GLStuff* gl = &win->gl;

    if (tigrGAPIBegin(bmp) < 0) {
        tigrError(bmp, "Cannot activate OpenGL context.\n");
        return;
    }

    if (!gl->gl_legacy) {
        glDeleteTextures(2, gl->tex);
        glDeleteProgram(gl->program);
    }

    tigrCheckGLError("destroy");

    if (tigrGAPIEnd(bmp) < 0) {
        tigrError(bmp, "Cannot deactivate OpenGL context.\n");
        return;
    }
}

void tigrGAPIDraw(int legacy, GLuint uniform_model, GLuint tex, Tigr* bmp, int x1, int y1, int x2, int y2) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bmp->w, bmp->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp->pix);

    if (!legacy) {
        float sx = (float)(x2 - x1);
        float sy = (float)(y2 - y1);
        float tx = (float)x1;
        float ty = (float)y1;

        float model[16] = { sx, 0.0f, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, tx, ty, 0.0f, 1.0f };

        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    } else {
#if !(__APPLE__ || __ANDROID__)
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(x2, y1);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(x1, y1);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(x1, y2);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(x2, y2);
        glEnd();
#else
        assert(0);
#endif
    }
}

void tigrGAPIPresent(Tigr* bmp, int w, int h) {
    TigrInternal* win = tigrInternal(bmp);
    GLStuff* gl = &win->gl;

    glViewport(0, 0, w, h);
    if (!gl->gl_user_opengl_rendering) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    if (!gl->gl_legacy) {
        float projection[16] = { 2.0f / w, 0.0f, 0.0f, 0.0f, 0.0f,  -2.0f / h, 0.0f, 0.0f,
                                 0.0f,     0.0f, 1.0f, 0.0f, -1.0f, 1.0f,      0.0f, 1.0f };

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(gl->vao);
        glUseProgram(gl->program);
        glUniformMatrix4fv(gl->uniform_projection, 1, GL_FALSE, projection);
        glUniform4f(gl->uniform_parameters, win->p1, win->p2, win->p3, win->p4);
    } else {
#if !(__APPLE__ || __ANDROID__)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
#else
        assert(0);
#endif
    }

    if (gl->gl_user_opengl_rendering) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
    tigrGAPIDraw(gl->gl_legacy, gl->uniform_model, gl->tex[0], bmp, win->pos[0], win->pos[1], win->pos[2], win->pos[3]);

    if (win->widgetsScale > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        tigrGAPIDraw(gl->gl_legacy, gl->uniform_model, gl->tex[1], win->widgets,
                     (int)(w - win->widgets->w * win->widgetsScale), 0, w, (int)(win->widgets->h * win->widgetsScale));
    }

    tigrCheckGLError("present");

    gl->gl_user_opengl_rendering = 0;
}

#endif
#endif // #ifndef TIGR_HEADLESS
//////// End of inlined file: tigr_gl.c ////////

//////// Start of inlined file: tigr_utils.c ////////

//#include "tigr_internal.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef __ANDROID__

#ifdef __IOS__
void* _tigrReadFile(const char* fileName, int* length) {
#else
void* tigrReadFile(const char* fileName, int* length) {
#endif
    // TODO - unicode?
    FILE* file;
    char* data;
    size_t len;

    if (length)
        *length = 0;

    file = fopen(fileName, "rb");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);

    data = (char*)malloc(len + 1);
    if (!data) {
        fclose(file);
        return NULL;
    }

    if (fread(data, 1, len, file) != len) {
        free(data);
        fclose(file);
        return NULL;
    }
    data[len] = '\0';
    fclose(file);

    if (length)
        *length = len;

    return data;
}

#endif  // __ANDROID__

// Reads a single UTF8 codepoint.
const char* tigrDecodeUTF8(const char* text, int* cp) {
    unsigned char c = *text++;
    int extra = 0, min = 0;
    *cp = 0;
    if (c >= 0xf0) {
        *cp = c & 0x07;
        extra = 3;
        min = 0x10000;
    } else if (c >= 0xe0) {
        *cp = c & 0x0f;
        extra = 2;
        min = 0x800;
    } else if (c >= 0xc0) {
        *cp = c & 0x1f;
        extra = 1;
        min = 0x80;
    } else if (c >= 0x80) {
        *cp = 0xfffd;
    } else {
        *cp = c;
    }
    while (extra--) {
        c = *text++;
        if ((c & 0xc0) != 0x80) {
            *cp = 0xfffd;
            break;
        }
        (*cp) = ((*cp) << 6) | (c & 0x3f);
    }
    if (*cp < min) {
        *cp = 0xfffd;
    }
    return text;
}

char* tigrEncodeUTF8(char* text, int cp) {
    if (cp < 0 || cp > 0x10ffff) {
        cp = 0xfffd;
    }

#define EMIT(X, Y, Z) *text++ = X | ((cp >> Y) & Z)
    if (cp < 0x80) {
        EMIT(0x00, 0, 0x7f);
    } else if (cp < 0x800) {
        EMIT(0xc0, 6, 0x1f);
        EMIT(0x80, 0, 0x3f);
    } else if (cp < 0x10000) {
        EMIT(0xe0, 12, 0xf);
        EMIT(0x80, 6, 0x3f);
        EMIT(0x80, 0, 0x3f);
    } else {
        EMIT(0xf0, 18, 0x7);
        EMIT(0x80, 12, 0x3f);
        EMIT(0x80, 6, 0x3f);
        EMIT(0x80, 0, 0x3f);
    }
    return text;
#undef EMIT
}

#ifndef TIGR_HEADLESS

int tigrBeginOpenGL(Tigr* bmp) {
#ifdef TIGR_GAPI_GL
    TigrInternal* win = tigrInternal(bmp);
    win->gl.gl_user_opengl_rendering = 1;
    return tigrGAPIBegin(bmp) == 0;
#else
    return 0;
#endif
}

void tigrSetPostShader(Tigr* bmp, const char* code, int size) {
#ifdef TIGR_GAPI_GL
    tigrGAPIBegin(bmp);
    TigrInternal* win = tigrInternal(bmp);
    GLStuff* gl = &win->gl;
    tigrCreateShaderProgram(gl, code, size);
    tigrGAPIEnd(bmp);
#endif
}

void tigrSetPostFX(Tigr* bmp, float p1, float p2, float p3, float p4) {
    TigrInternal* win = tigrInternal(bmp);
    win->p1 = p1;
    win->p2 = p2;
    win->p3 = p3;
    win->p4 = p4;
}

#endif // TIGR_HEADLESS

//////// End of inlined file: tigr_utils.c ////////


//////// End of inlined file: tigr_amalgamated.c ////////

