#!/bin/sh

set -xe

COMMON_CFLAGS="-Wall -Wextra -ggdb -I. -I./build/ -I./thirdparty/"

build_vc_demo() {
    NAME=$1

    clang $COMMON_CFLAGS -Os -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--export=init -Wl,--allow-undefined -o ./build/$NAME.wasm -DPLATFORM=WASM_PLATFORM ./demos/$NAME.c
    cp ./build/$NAME.wasm ./wasm/
    clang $COMMON_CFLAGS -o ./build/$NAME.sdl -DPLATFORM=SDL_PLATFORM ./demos/$NAME.c -lm -lSDL2
    clang $COMMON_CFLAGS -o ./build/$NAME.term -DPLATFORM=TERM_PLATFORM ./demos/$NAME.c -lm
}

mkdir -p ./build/

# Build tests
clang $COMMON_CFLAGS -o ./build/test -Ithirdparty test.c -lm

# Build asserts
clang $COMMON_CFLAGS -o ./build/png2c -Ithirdparty png2c.c -lm
mkdir -p ./build/assets/
./build/png2c ./assets/tsodinPog.png > ./build/assets/tsodinPog.c

# Build VC demos
build_vc_demo triangle &
build_vc_demo 3d &
build_vc_demo squish &
wait # TODO: the whole script must fail if one of the jobs fails
