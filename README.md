# Olive.c

**IMPORTANT! THIS LIBRARY IS A WORK IN PROGRESS! ANYTHING CAN CHANGE AT ANY MOMENT WITHOUT ANY NOTICE! USE THIS LIBRARY AT YOUR OWN RISK!**

Simple graphics library that does not have any dependencies and renders everything into the given memory pixel by pixel.

Visit [https://tsoding.org/olive.c/](https://tsoding.org/olive.c/) to see some demos.

The library is not concerned with displaying the image. It only fills up the memory with pixels. It's up to you what to do with those pixels.

The name is pronounced as "olivets'" which is a Ukrainian word for "pencil" (["олівець"](https://translate.google.com/?sl=uk&tl=en&text=%D0%BE%D0%BB%D1%96%D0%B2%D0%B5%D1%86%D1%8C&op=translate)).

The library itself does not require any special building. You can simply copy-paste [./olive.c](./olive.c) to your project and `#include` it. (Because the truly reusable code is the one that you can simply copy-paste).

Olive.c is a classical [stb-style](https://github.com/nothings/stb) single header library. That is by default it acts like a header, but if you `#define OLIVEC_IMPLEMENTATION` prior including the library it acts like a source file. The `.c` extension might be confusing, but it's a part of the name of the library (why JavaScript people can use `.js` as part of the name of a library and I cannot?)

## Quick Example (Flag of Japan)

<!-- TODO: get rid of the dependency on stb_image_write.h in here and replace it with PPM format -->

*This example also uses [stb_image_write.h](https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h) to create the PNG image*

```c
// flag_jp.c
#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 900
#define HEIGHT 600

uint32_t pixels[WIDTH*HEIGHT];

int main(void)
{
    Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT);
    // Taken from https://upload.wikimedia.org/wikipedia/en/9/9e/Flag_of_Japan.svg
    olivec_fill(oc, 0xFFFFFFFF);
    olivec_circle(oc, WIDTH/2, HEIGHT/2, 180, 0xFF2D00BC);

    const char *file_path = "flag_jp.png";
    if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t)*WIDTH)) {
        fprintf(stderr, "ERROR: could not write %s\n", file_path);
        return 1;
    }
    return 0;
}
```

## Building the Tests and Demos

Even though the library does not require any special building, the tests and demos do. Just execute `build.sh` script to build everything.

```console
$ ./build.sh
```

Run the tests:

```console
$ ./build/test run
```

If the expected behavior of the library has changed in the way that breaks current test cases, you probably want to update them:

```console
$ ./build/test record
```

For more info see the help:

```console
$ ./build/test help
```

## Demos

The source code for demos is located at [demos](./demos/). Each demo is compiled for 3 different "platforms" that is 3 different ways to display the generated images:

1. [SDL](https://www.libsdl.org/) -- displays the images via [SDL_Texture](https://wiki.libsdl.org/SDL_Texture).
2. Terminal -- converts the images into ASCII art and prints them into the terminal.
3. WASM -- displays the images in [HTML5 canvas](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)

To run the SDL version of a demo do

```console
$ ./build/<demo>.sdl
```

To run the Terminal version of a demo do

```console
$ ./build/<demo>.term
```

To run the WASM versions of the demos from [https://tsoding.org/olive.c/](https://tsoding.org/olive.c/) locally do

```console
$ python3 -m http.server 6969
$ iexplore.exe http://localhost:6969/
```

### Virtual Console

The support for several platforms is provided by Demo Virtual Console. It is implemented in two files:

- [./demos/vc.c](./demos/vc.c) -- the C runtime required by all platforms.
- [./js/vc.js](./js/vc.js) -- the JavaScript runtime for running in a browser when compiled to WebAssembly.

The Demo Virtual Console is not part of the main library and is designed specifically for demos. (I do consider including it into the main library, 'cause it looks pretty useful. The library is still in development).
