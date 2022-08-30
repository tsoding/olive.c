# Olive.c

Simple 2D Graphics Library for C.

## Quick Start

### Test the Library

```console
$ ./build.sh
```

### Use the Library

> The truly reusable code is the one that you can simply copy-paste.

Copy-paste [./olive.c](./olive.c) to your project.

```c
// flag_jp.c
#include "olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 900
#define HEIGHT 600

uint32_t pixels[WIDTH*HEIGHT];

int main(void)
{
    // Taken from https://upload.wikimedia.org/wikipedia/en/9/9e/Flag_of_Japan.svg
    olivec_fill(pixels, WIDTH, HEIGHT, 0xFFFFFFFF);
    olivec_fill_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 180, 0xFF2D00BC);

    const char *file_path = "flag_jp.png";
    if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t)*WIDTH)) {
        fprintf(stderr, "ERROR: could not write %s\n", file_path);
        return 1;
    }
    return 0;
}
```

## Gallery

![checker](./imgs/checker.png)

![circle](./imgs/circle.png)

![lines](./imgs/lines.png)
