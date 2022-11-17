#!/bin/sh

set -xe

COMMON_CFLAGS="-Wall -Wextra -pedantic -std=c99 -ggdb -I. -I./build/ -I./dev-deps/"

build_wasm_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--export=__heap_base -Wl,--allow-undefined -o ./build/demos/$NAME.wasm -DPLATFORM=WASM_PLATFORM ./demos/$NAME.c
    cp ./build/demos/$NAME.wasm ./wasm/
}

build_term_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -o ./build/demos/$NAME.term -DPLATFORM=TERM_PLATFORM -D_XOPEN_SOURCE=600 ./demos/$NAME.c -lm
}

build_sdl_demo() {
    NAME=$1
    clang $COMMON_CFLAGS -O2 -o ./build/demos/$NAME.sdl -I/usr/include/SDL2 -DPLATFORM=SDL_PLATFORM ./demos/$NAME.c -lm -lSDL2
}

build_vc_demo() {
    NAME=$1
    build_wasm_demo $NAME
    build_term_demo $NAME
    build_sdl_demo $NAME
}

build_all_vc_demos() {
    mkdir -p ./build/demos
    build_vc_demo triangle &
    build_vc_demo 3d &
    build_vc_demo squish &
    build_vc_demo triangle3d &
    build_vc_demo triangleTex &
    build_vc_demo triangle3dTex &
    build_vc_demo cup3d &
    wait # TODO: the whole script must fail if one of the jobs fails
}

build_tools() {
    mkdir -p ./build/tools/
    clang $COMMON_CFLAGS -o ./build/tools/png2c -Ithirdparty ./tools/png2c.c -lm &
    clang $COMMON_CFLAGS -o ./build/tools/obj2c -Ithirdparty ./tools/obj2c.c -lm &
    clang $COMMON_CFLAGS -O2 -o ./build/tools/viewobj ./tools/viewobj.c &
    wait # TODO: the whole script must fail if one of the jobs fails
}

build_assets() {
    mkdir -p ./build/assets/
    ./build/tools/png2c -n tsodinPog -o ./build/assets/tsodinPog.c ./assets/tsodinPog.png &
    ./build/tools/png2c -n Sadge -o ./build/assets/Sadge.c ./assets/Sadge.png &
    ./build/tools/obj2c ./assets/tsodinCupLowPoly.obj ./build/assets/tsodinCupLowPoly.c &
    wait # TODO: the whole script must fail if one of the jobs fails
}

build_tests() {
    clang $COMMON_CFLAGS -fsanitize=memory -o ./build/test -Ithirdparty ./test.c -lm &
}

build_tools
build_assets
build_tests
build_all_vc_demos
