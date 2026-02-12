# Olive.c – Android Example

<p align="center">
<img src="../assets/olivec-200.png" width="160">
</p>

Minimal Android demo using **Olive.c** as a CPU renderer.

## Idea

Olive.c renders directly into the memory of an Android `Bitmap`.

* lock bitmap → get pixels pointer
* use pointer as `Olivec_Canvas`
* draw
* unlock → display

No copies. No return values. The bitmap memory is the render target.

## Loop

* refresh every ~10 ms
* render frame
* apply rotation

## Native (pseudo)

```c
void *bitmapPixels;
AndroidBitmap_lockPixels(env, bmp, &bitmapPixels);
memcpy(bitmapPixels, pixels, sizeof(pixels));
AndroidBitmap_unlockPixels(env, bmp);
```