#define NOBUILD_IMPLEMENTATION
#include "./nobuild.h"

#define COMMON_CFLAGS "-Wall", "-Wextra", "-pedantic", "-std=c99", "-ggdb", "-I.", "-I./build/", "-I./dev-deps/"

// TODO: implement parallel building

void build_tools(void)
{
    MKDIRS("build", "tools");
    CMD("clang", COMMON_CFLAGS, "-o", "./build/tools/png2c", "./tools/png2c.c", "-lm");
    CMD("clang", COMMON_CFLAGS, "-o", "./build/tools/obj2c", "./tools/obj2c.c", "-lm");
}

void build_assets(void)
{
    MKDIRS("build", "assets");
    CMD("./build/tools/png2c", "-n", "tsodinPog", "-o", "./build/assets/tsodinPog.c", "./assets/tsodinPog.png");
    CMD("./build/tools/png2c", "-n", "tsodinCup", "-o", "./build/assets/tsodinCup.c", "./assets/tsodinCup.png");
    CMD("./build/tools/obj2c", "-o", "./build/assets/tsodinCupLowPoly.c", "./assets/tsodinCupLowPoly.obj");
    CMD("./build/tools/obj2c", "-s", "0.40", "-o", "./build/assets/utahTeapot.c", "./assets/utahTeapot.obj");
}

void build_tests(void)
{
    CMD("clang", COMMON_CFLAGS, "-fsanitize=memory", "-o", "./build/test", "test.c", "-lm");
}

// TODO: move copy_file to nobuild.h
// Maybe even use platform dependent APIs to make it better.
void copy_file(const char *src_file_path, const char *dst_file_path)
{
    INFO("Copying %s -> %s", src_file_path, dst_file_path);
    size_t buffer_sz = 32*1024;
    char *buffer = malloc(buffer_sz);
    if (buffer == NULL) {
        PANIC("Could not allocate memory to copy file %s -> %s", src_file_path, dst_file_path);
    }

    FILE *src = fopen(src_file_path, "rb");
    if (src == NULL) {
        PANIC("Could not open file %s for reading: %s", src_file_path, strerror(errno));
    }
    FILE *dst = fopen(dst_file_path, "wb");
    if (dst == NULL) {
        PANIC("Could not open file %s for writing: %s", dst_file_path, strerror(errno));
    }
    while (!feof(src)) {
        size_t n = fread(buffer, 1, buffer_sz, src);
        if (ferror(src)) {
            PANIC("Could not read from file %s: %s", src_file_path, strerror(errno));
        }
        size_t m = 0;
        while (m < n) {
            m += fwrite(buffer + m, 1, n - m, dst);
            if (ferror(dst)) {
                PANIC("Could not write to file %s: %s", dst_file_path, strerror(errno));
            }
        }
    }
    if (fclose(dst) < 0) PANIC("Could not close file %s: %s", dst_file_path, strerror(errno));
    if (fclose(src) < 0) PANIC("Could not close file %s: %s", src_file_path, strerror(errno));
}

void build_wasm_demo(const char *name)
{
    CMD("clang", COMMON_CFLAGS, "-O2", "-fno-builtin", "--target=wasm32", "--no-standard-libraries", "-Wl,--no-entry", "-Wl,--export=vc_render", "-Wl,--export=__heap_base", "-Wl,--allow-undefined", "-o", CONCAT("./build/demos/", name, ".wasm"), "-DVC_PLATFORM=VC_WASM_PLATFORM", CONCAT("./demos/", name, ".c"));
    copy_file(CONCAT("./build/demos/", name, ".wasm"), CONCAT("./wasm/", name, ".wasm"));
}

void build_term_demo(const char *name)
{
    CMD("clang", COMMON_CFLAGS, "-O2", "-o", CONCAT("./build/demos/", name, ".term"), "-DVC_PLATFORM=VC_TERM_PLATFORM", "-D_XOPEN_SOURCE=600", CONCAT("./demos/", name, ".c"), "-lm");
}

void build_sdl_demo(const char *name)
{
    CMD("clang", COMMON_CFLAGS, "-O2", "-o", CONCAT("./build/demos/", name, ".sdl"), "-DVC_PLATFORM=VC_SDL_PLATFORM", CONCAT("./demos/", name, ".c"), "-lm", "-lSDL2");
}

void build_vc_demo(const char *name)
{
    build_wasm_demo(name);
    build_term_demo(name);
    build_sdl_demo(name);
}

void build_all_vc_demos(void)
{
    MKDIRS("build", "demos");
    build_vc_demo("triangle");
    build_vc_demo("dots3d");
    build_vc_demo("squish");
    build_vc_demo("triangle3d");
    build_vc_demo("triangleTex");
    build_vc_demo("triangle3dTex");
    build_vc_demo("cup3d");
    build_vc_demo("teapot3d");
}

int main(int argc, char **argv)
{
    GO_REBUILD_URSELF(argc, argv);

    const char *program = shift_args(&argc, &argv);

    if (argc > 0) {
        const char *subcmd = shift_args(&argc, &argv);
        if (strcmp(subcmd, "tools") == 0) {
            // TODO: rebuild specific tool
            build_tools();
        } else if (strcmp(subcmd, "assets") == 0) {
            // TODO: rebuild specific assets
            build_assets();
        } else if (strcmp(subcmd, "tests") == 0) {
            // TODO: pass arguments to the ./build/test command
            build_tests();
        } else if (strcmp(subcmd, "demos") == 0) {
            // TODO: build specific demos for the specific platforms
            build_all_vc_demos();
        } else {
            PANIC("Unknown command `%s`", subcmd);
        }
    } else {
        build_tools();
        build_assets();
        build_tests();
        build_all_vc_demos();
    } 

    return 0;
}
