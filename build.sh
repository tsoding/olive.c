#!/bin/sh

set -xe

COMMON_CFLAGS="-Wall -Wextra -ggdb -I. -I./build/ -I./thirdparty/"

build_vc_demo() {
    NAME=$1

    clang $COMMON_CFLAGS -O2 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--export=__heap_base -Wl,--allow-undefined -o ./build/demos/$NAME.wasm -DPLATFORM=WASM_PLATFORM ./demos/$NAME.c
    cp ./build/demos/$NAME.wasm ./wasm/
    clang $COMMON_CFLAGS -O2 -o ./build/demos/$NAME.sdl -DPLATFORM=SDL_PLATFORM ./demos/$NAME.c -lm -lSDL2
    clang $COMMON_CFLAGS -O2 -o ./build/demos/$NAME.term -DPLATFORM=TERM_PLATFORM ./demos/$NAME.c -lm
}

build_all_vc_demos() {
    mkdir -p ./build/demos
    build_vc_demo triangle &
    build_vc_demo 3d &
    build_vc_demo squish &
    build_vc_demo triangle3d &
    build_vc_demo triangleTex &
    build_vc_demo triangle3dTex &
    wait # TODO: the whole script must fail if one of the jobs fails
}

build_assets() {
    mkdir -p ./build/assets/
    clang $COMMON_CFLAGS -o ./build/png2c -Ithirdparty png2c.c -lm
    ./build/png2c -n tsodinPog -o ./build/assets/tsodinPog.c ./assets/tsodinPog.png
    ./build/png2c -n Sadge -o ./build/assets/Sadge.c ./assets/Sadge.png
}

build_tests() {
    mkdir -p ./build/
    clang $COMMON_CFLAGS -fsanitize=memory -o ./build/test -Ithirdparty test.c -lm
}

build_assets
build_tests
build_all_vc_demos
