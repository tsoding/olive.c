// TIGR - TIny GRaphics Library - v3.1
//        ^^   ^^
//
// rawr.

/*
This is free and unencumbered software released into the public domain.

Our intent is that anyone is free to copy and use this software,
for any purpose, in any form, and by any means.

The authors dedicate any and all copyright interest in the software
to the public domain, at their own expense for the betterment of mankind.

The software is provided "as is", without any kind of warranty, including
any implied warranty. If it breaks, you get to keep both pieces.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Compiler configuration.
#ifdef _MSC_VER
#define TIGR_INLINE static __forceinline
#else
#define TIGR_INLINE static inline
#endif

// Bitmaps ----------------------------------------------------------------

// This struct contains one pixel.
typedef struct {
    unsigned char r, g, b, a;
} TPixel;

// Window flags.
#define TIGR_FIXED      0   // window's bitmap is a fixed size (default)
#define TIGR_AUTO       1   // window's bitmap is scaled with the window
#define TIGR_2X         2   // always enforce (at least) 2X pixel scale
#define TIGR_3X         4   // always enforce (at least) 3X pixel scale
#define TIGR_4X         8   // always enforce (at least) 4X pixel scale
#define TIGR_RETINA     16  // enable retina support on OS X
#define TIGR_NOCURSOR   32  // hide cursor
#define TIGR_FULLSCREEN 64  // start in full-screen mode

// A Tigr bitmap.
typedef struct Tigr {
    int w, h;           // width/height (unscaled)
    int cx, cy, cw, ch; // clip rect
    TPixel *pix;        // pixel data
    void *handle;       // OS window handle, NULL for off-screen bitmaps.
    int blitMode;       // Target bitmap blit mode
} Tigr;

// Creates a new empty window with a given bitmap size.
//
// Title is UTF-8.
//
// In TIGR_FIXED mode, the window is made as large as possible to contain an integer-scaled
// version of the bitmap while still fitting on the screen. Resizing the window will adapt
// the scale in integer steps to fit the bitmap.
//
// In TIGR_AUTO mode, the initial window size is set to the bitmap size times the pixel
// scale. Resizing the window will resize the bitmap using the specified scale.
// For example, in forced 2X mode, the window will be twice as wide (and high) as the bitmap.
//
// Turning on TIGR_RETINA mode will request full backing resolution on OSX, meaning that
// the effective window size might be integer scaled to a larger size. In TIGR_AUTO mode,
// this means that the Tigr bitmap will change size if the window is moved between
// retina and non-retina screens.
//
Tigr *tigrWindow(int w, int h, const char *title, int flags);

// Creates an empty off-screen bitmap.
Tigr *tigrBitmap(int w, int h);

// Deletes a window/bitmap.
void tigrFree(Tigr *bmp);

// Returns non-zero if the user requested to close a window.
int tigrClosed(Tigr *bmp);

// Displays a window's contents on-screen and updates input.
void tigrUpdate(Tigr *bmp);

// Called before doing direct OpenGL calls and before tigrUpdate.
// Returns non-zero if OpenGL is available.
int tigrBeginOpenGL(Tigr *bmp);

// Sets post shader for a window.
// This replaces the built-in post-FX shader.
void tigrSetPostShader(Tigr *bmp, const char* code, int size);

// Sets post-FX properties for a window.
//
// The built-in post-FX shader uses the following parameters:
// p1: hblur - use bilinear filtering along the x-axis (pixels)
// p2: vblur - use bilinear filtering along the y-axis (pixels)
// p3: scanlines - CRT scanlines effect (0-1)
// p4: contrast - contrast boost (1 = no change, 2 = 2X contrast, etc)
void tigrSetPostFX(Tigr *bmp, float p1, float p2, float p3, float p4);


// Drawing ----------------------------------------------------------------

// Helper for reading pixels.
// For high performance, just access bmp->pix directly.
TPixel tigrGet(Tigr *bmp, int x, int y);

// Plots a pixel.
// Clips and blends.
// For high performance, just access bmp->pix directly.
void tigrPlot(Tigr *bmp, int x, int y, TPixel pix);

// Clears a bitmap to a color.
// No blending, no clipping.
void tigrClear(Tigr *bmp, TPixel color);

// Fills a rectangular area.
// No blending, no clipping.
void tigrFill(Tigr *bmp, int x, int y, int w, int h, TPixel color);

// Draws a line.
// Start pixel is drawn, end pixel is not.
// Clips and blends.
void tigrLine(Tigr *bmp, int x0, int y0, int x1, int y1, TPixel color);

// Draws an empty rectangle.
// Drawing a 1x1 rectangle yields the same result as calling tigrPlot.
// Clips and blends.
void tigrRect(Tigr *bmp, int x, int y, int w, int h, TPixel color);

// Fills a rectangle.
// Fills the inside of the specified rectangular area.
// Calling tigrRect followed by tigrFillRect using the same arguments
// causes no overdrawing.
// Clips and blends.
void tigrFillRect(Tigr *bmp, int x, int y, int w, int h, TPixel color);

// Draws a circle.
// Drawing a zero radius circle yields the same result as calling tigrPlot.
// Drawing a circle with radius one draws a circle three pixels wide.
// Clips and blends.
void tigrCircle(Tigr *bmp, int x, int y, int r, TPixel color);

// Fills a circle.
// Fills the inside of the specified circle.
// Calling tigrCircle followed by tigrFillCircle using the same arguments
// causes no overdrawing.
// Filling a circle with zero radius has no effect.
// Clips and blends.
void tigrFillCircle(Tigr *bmp, int x, int y, int r, TPixel color);

// Sets clip rect.
// Set to (0, 0, -1, -1) to reset clipping to full bitmap.
void tigrClip(Tigr *bmp, int cx, int cy, int cw, int ch);

// Copies bitmap data.
// dx/dy = dest co-ordinates
// sx/sy = source co-ordinates
// w/h   = width/height
//
// RGBAdest = RGBAsrc
// Clips, does not blend.
void tigrBlit(Tigr *dest, Tigr *src, int dx, int dy, int sx, int sy, int w, int h);

// Same as tigrBlit, but alpha blends the source bitmap with the
// target using per pixel alpha and the specified global alpha.
//
// Ablend = Asrc * alpha
// RGBdest = RGBsrc * Ablend + RGBdest * (1 - Ablend)
//
// Blit mode == TIGR_KEEP_ALPHA:
// Adest = Adest
//
// Blit mode == TIGR_BLEND_ALPHA:
// Adest = Asrc * Ablend + Adest * (1 - Ablend)
// Clips and blends.
void tigrBlitAlpha(Tigr *dest, Tigr *src, int dx, int dy, int sx, int sy, int w, int h, float alpha);

// Same as tigrBlit, but tints the source bitmap with a color
// and alpha blends the resulting source with the destination.
//
// Rblend = Rsrc * Rtint
// Gblend = Gsrc * Gtint
// Bblend = Bsrc * Btint
// Ablend = Asrc * Atint
//
// RGBdest = RGBblend * Ablend + RGBdest * (1 - Ablend)
//
// Blit mode == TIGR_KEEP_ALPHA:
// Adest = Adest
//
// Blit mode == TIGR_BLEND_ALPHA:
// Adest = Ablend * Ablend + Adest * (1 - Ablend)
// Clips and blends.
void tigrBlitTint(Tigr *dest, Tigr *src, int dx, int dy, int sx, int sy, int w, int h, TPixel tint);

enum TIGRBlitMode {
    TIGR_KEEP_ALPHA = 0,    // Keep destination alpha value
    TIGR_BLEND_ALPHA = 1,   // Blend destination alpha (default)
};

// Set destination bitmap blend mode for blit operations.
void tigrBlitMode(Tigr *dest, int mode);

// Helper for making colors.
TIGR_INLINE TPixel tigrRGB(unsigned char r, unsigned char g, unsigned char b)
{
    TPixel p; p.r = r; p.g = g; p.b = b; p.a = 0xff; return p;
}

// Helper for making colors.
TIGR_INLINE TPixel tigrRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    TPixel p; p.r = r; p.g = g; p.b = b; p.a = a; return p;
}


// Font printing ----------------------------------------------------------

typedef struct {
    int code, x, y, w, h;
} TigrGlyph;

typedef struct {
    Tigr *bitmap;
    int numGlyphs;
    TigrGlyph *glyphs;
} TigrFont;

typedef enum {
    TCP_ASCII = 0,
    TCP_1252 = 1252,
    TCP_UTF32 = 12001
} TCodepage;

// Loads a font.
//
// Codepages:
//
//  TCP_ASCII   - Regular 7-bit ASCII
//  TCP_1252    - Windows 1252
//  TCP_UTF32   - Unicode subset
//
// For ASCII and 1252, the font bitmap should contain all characters
// for the given codepage, excluding the first 32 control codes.
//
// For UTF32 - the font bitmap contains a subset of Unicode characters
// and must be in the format generated by tigrFont for UTF32.
//
TigrFont *tigrLoadFont(Tigr *bitmap, int codepage);

// Frees a font.
void tigrFreeFont(TigrFont *font);

// Prints UTF-8 text onto a bitmap.
// NOTE:
//  This uses the target bitmap blit mode.
//  See tigrBlitTint for details.
void tigrPrint(Tigr *dest, TigrFont *font, int x, int y, TPixel color, const char *text, ...);

// Returns the width/height of a string.
int tigrTextWidth(TigrFont *font, const char *text);
int tigrTextHeight(TigrFont *font, const char *text);

// The built-in font.
extern TigrFont *tfont;


// User Input -------------------------------------------------------------

// Key scancodes. For letters/numbers, use ASCII ('A'-'Z' and '0'-'9').
typedef enum {
    TK_PAD0=128,TK_PAD1,TK_PAD2,TK_PAD3,TK_PAD4,TK_PAD5,TK_PAD6,TK_PAD7,TK_PAD8,TK_PAD9,
    TK_PADMUL,TK_PADADD,TK_PADENTER,TK_PADSUB,TK_PADDOT,TK_PADDIV,
    TK_F1,TK_F2,TK_F3,TK_F4,TK_F5,TK_F6,TK_F7,TK_F8,TK_F9,TK_F10,TK_F11,TK_F12,
    TK_BACKSPACE,TK_TAB,TK_RETURN,TK_SHIFT,TK_CONTROL,TK_ALT,TK_PAUSE,TK_CAPSLOCK,
    TK_ESCAPE,TK_SPACE,TK_PAGEUP,TK_PAGEDN,TK_END,TK_HOME,TK_LEFT,TK_UP,TK_RIGHT,TK_DOWN,
    TK_INSERT,TK_DELETE,TK_LWIN,TK_RWIN,TK_NUMLOCK,TK_SCROLL,TK_LSHIFT,TK_RSHIFT,
    TK_LCONTROL,TK_RCONTROL,TK_LALT,TK_RALT,TK_SEMICOLON,TK_EQUALS,TK_COMMA,TK_MINUS,
    TK_DOT,TK_SLASH,TK_BACKTICK,TK_LSQUARE,TK_BACKSLASH,TK_RSQUARE,TK_TICK
} TKey;

// Returns mouse input for a window.
void tigrMouse(Tigr *bmp, int *x, int *y, int *buttons);

typedef struct {
    int x;
    int y;
} TigrTouchPoint;

// Reads touch input for a window.
// Returns number of touch points read.
int tigrTouch(Tigr *bmp, TigrTouchPoint* points, int maxPoints);

// Reads the keyboard for a window.
// Returns non-zero if a key is pressed/held.
// tigrKeyDown tests for the initial press, tigrKeyHeld repeats each frame.
int tigrKeyDown(Tigr *bmp, int key);
int tigrKeyHeld(Tigr *bmp, int key);

// Reads character input for a window.
// Returns the Unicode value of the last key pressed, or 0 if none.
int tigrReadChar(Tigr *bmp);

// Show / hide virtual keyboard.
// (Only available on iOS / Android)
void tigrShowKeyboard(int show);


// Bitmap I/O -------------------------------------------------------------

// Loads a PNG, from either a file or memory. (fileName is UTF-8)
// On error, returns NULL and sets errno.
Tigr *tigrLoadImage(const char *fileName);
Tigr *tigrLoadImageMem(const void *data, int length);

// Saves a PNG to a file. (fileName is UTF-8)
// On error, returns zero and sets errno.
int tigrSaveImage(const char *fileName, Tigr *bmp);


// Helpers ----------------------------------------------------------------

// Returns the amount of time elapsed since tigrTime was last called,
// or zero on the first call.
float tigrTime(void);

// Displays an error message and quits. (UTF-8)
// 'bmp' can be NULL.
void tigrError(Tigr *bmp, const char *message, ...);

// Reads an entire file into memory. (fileName is UTF-8)
// Free it yourself after with 'free'.
// On error, returns NULL and sets errno.
// TIGR will automatically append a NUL terminator byte
// to the end (not included in the length)
void *tigrReadFile(const char *fileName, int *length);

// Decompresses DEFLATEd zip/zlib data into a buffer.
// Returns non-zero on success.
int tigrInflate(void *out, unsigned outlen, const void *in, unsigned inlen);

// Decodes a single UTF8 codepoint and returns the next pointer.
const char *tigrDecodeUTF8(const char *text, int *cp);

// Encodes a single UTF8 codepoint and returns the next pointer.
char *tigrEncodeUTF8(char *text, int cp);

#ifdef __cplusplus
}
#endif
