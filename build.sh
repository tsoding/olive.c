#!/bin/sh

set -xe

mkdir -p ./bin/

clang -Wall -Wextra -ggdb -o ./bin/test -Ithirdparty test.c -lm
clang -Wall -Wextra -ggdb -o ./bin/gallery -Ithirdparty -I. examples/gallery.c
clang -Os -fno-builtin -Wall -Wextra -Wswitch-enum --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--allow-undefined  -o ./bin/triangle.wasm -I. ./examples/triangle.c
clang -Os -fno-builtin -Wall -Wextra -Wswitch-enum --target=wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export=render -Wl,--allow-undefined  -o ./bin/3d.wasm -I. ./examples/3d.c
clang -O2 -Wall -Wextra -ggdb -I. -DSDL_PLATFORM -o ./bin/triangle ./examples/triangle.c -lm -lSDL2
clang -O2 -Wall -Wextra -ggdb -I. -DSDL_PLATFORM -o ./bin/3d ./examples/3d.c -lm -lSDL2
