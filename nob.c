#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD
#include "./dev-deps/nob.h"

#define COMMON_CFLAGS "-Wall", "-Wextra", "-pedantic", "-ggdb", "-I.", "-I./build/", "-I./dev-deps/"

bool build_tools(Cmd *cmd)
{
    if (!mkdir_if_not_exists("build")) return false;
    if (!mkdir_if_not_exists("build/tools")) return false;

    cmd_append(cmd, "clang", COMMON_CFLAGS, "-o", "./build/tools/png2c", "./tools/png2c.c", "-lm");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "clang", COMMON_CFLAGS, "-o", "./build/tools/obj2c", "./tools/obj2c.c", "-lm");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    return true;
}

bool build_assets(Cmd *cmd)
{
    if (!mkdir_if_not_exists("build")) return false;
    if (!mkdir_if_not_exists("build/assets")) return false;

    cmd_append(cmd, "./build/tools/png2c", "-n", "tsodinPog", "-o", "./build/assets/tsodinPog.c", "./assets/tsodinPog.png");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "./build/tools/png2c", "-n", "tsodinCup", "-o", "./build/assets/tsodinCup.c", "./assets/tsodinCup.png");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "./build/tools/png2c", "-n", "oldstone", "-o", "./build/assets/oldstone.c", "./assets/oldstone.png");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "./build/tools/png2c", "-n", "lavastone", "-o", "./build/assets/lavastone.c", "./assets/lavastone.png");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "./build/tools/obj2c", "-o", "./build/assets/tsodinCupLowPoly.c", "./assets/tsodinCupLowPoly.obj");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "./build/tools/obj2c", "-s", "0.40", "-o", "./build/assets/utahTeapot.c", "./assets/utahTeapot.obj");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    cmd_append(cmd, "./build/tools/obj2c", "-s", "1.5", "-o", "./build/assets/penger.c", "./assets/penger_obj/penger.obj");
    if (!cmd_run_sync_and_reset(cmd)) return false;

    return true;
}

bool build_tests(Cmd *cmd)
{
    cmd_append(cmd, "clang", COMMON_CFLAGS, "-fsanitize=memory", "-o", "./build/test", "test.c", "-lm");
    if (!cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

void build_wasm_demo(Cmd *cmd, Procs *procs, const char *name)
{
    cmd_append(cmd, "clang", COMMON_CFLAGS, "-O2", "-fno-builtin", "--target=wasm32", "--no-standard-libraries", "-Wl,--no-entry", "-Wl,--export=vc_render", "-Wl,--export=__heap_base", "-Wl,--allow-undefined", "-o", temp_sprintf("./build/demos/%s.wasm", name), "-DVC_PLATFORM=VC_WASM_PLATFORM", temp_sprintf("./demos/%s.c", name));
    da_append(procs, cmd_run_async_and_reset(cmd));
}

void build_term_demo(Cmd *cmd, Procs *procs, const char *name)
{
    cmd_append(cmd, "clang", COMMON_CFLAGS, "-O2", "-o", temp_sprintf("./build/demos/%s.term", name), "-DVC_PLATFORM=VC_TERM_PLATFORM", "-D_XOPEN_SOURCE=600", temp_sprintf("./demos/%s.c", name), "-lm");
    da_append(procs, cmd_run_async_and_reset(cmd));
}

void build_sdl_demo(Cmd *cmd, Procs *procs, const char *name)
{
    cmd_append(cmd, "clang", COMMON_CFLAGS, "-O2", "-o", temp_sprintf("./build/demos/%s.sdl", name), "-DVC_PLATFORM=VC_SDL_PLATFORM", temp_sprintf("./demos/%s.c", name), "-lm", "-lSDL2", NULL);
    da_append(procs, cmd_run_async_and_reset(cmd));
}

void build_vc_demo(Cmd *cmd, Procs *procs, const char *name)
{
    build_wasm_demo(cmd, procs, name);
    build_term_demo(cmd, procs, name);
    build_sdl_demo(cmd, procs, name);
}

bool build_all_vc_demos(Cmd *cmd, Procs *procs)
{
    if (!mkdir_if_not_exists("build")) return false;
    if (!mkdir_if_not_exists("build/demos")) return false;

    const char *names[] = {
        "triangle",
        "dots3d",
        "squish",
        "triangle3d",
        "triangleTex",
        "triangle3dTex",
        "cup3d",
        "teapot3d",
        "penger3d",
    };
    size_t thread_count = 6;

    for (size_t i = 0; i < ARRAY_LEN(names); ++i) {
        build_vc_demo(cmd, procs, names[i]);
        if (procs->count >= thread_count) {
            if (!nob_procs_wait_and_reset(procs)) return false;
        }
    }
    if (!nob_procs_wait_and_reset(procs)) return false;

    for (size_t i = 0; i < ARRAY_LEN(names); ++i) {
        const char *src_path = temp_sprintf("./build/demos/%s.wasm", names[i]);
        const char *dst_path = temp_sprintf("./wasm/%s.wasm", names[i]);
        if (!copy_file(src_path, dst_path)) return false;
    }

    return true;
}

void usage(const char *program)
{
    nob_log(INFO, "Usage: %s [<subcommand>]", program);
    nob_log(INFO, "Subcommands:");
    nob_log(INFO, "    tools");
    nob_log(INFO, "        Build all the tools. Things like png2c, obj2c, etc.");
    nob_log(INFO, "    assets");
    nob_log(INFO, "        Build the assets in the assets/ folder.");
    nob_log(INFO, "        Basically convert their data to C code so we can bake them in demos.");
    nob_log(INFO, "    test[s] [<args>]");
    nob_log(INFO, "        Build and run test.c");
    nob_log(INFO, "        If <args> are provided the test utility is run with them.");
    nob_log(INFO, "    demos [<platform>] [run]");
    nob_log(INFO, "        Build demos.");
    nob_log(INFO, "        Available platforms are: sdl, term, or wasm.");
    nob_log(INFO, "        Optional [run] runs the demo after the build.");
    nob_log(INFO, "        [run] is not available for wasm platform.");
    nob_log(INFO, "    help");
    nob_log(INFO, "         Print this message");
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    Cmd cmd = {0};
    Procs procs = {0};

    const char *program = shift_args(&argc, &argv);

    if (argc > 0) {
        const char *subcmd = shift_args(&argc, &argv);
        if (strcmp(subcmd, "tools") == 0) {
            if (!build_tools(&cmd)) return 1;
        } else if (strcmp(subcmd, "assets") == 0) {
            if (!build_assets(&cmd)) return 1;
        } else if (strcmp(subcmd, "tests") == 0 || strcmp(subcmd, "test") == 0) {
            if (!build_tests(&cmd)) return 1;
            if (argc > 0) {
                cmd_append(&cmd, "./build/test");
                da_append_many(&cmd, argv, argc);
                if (!cmd_run_sync_and_reset(&cmd)) return 1;
            }
        } else if (strcmp(subcmd, "demos") == 0) {
            if (argc <= 0) {
                if (!build_all_vc_demos(&cmd, &procs)) return 1;
                return 0;
            }

            const char *name = shift(argv, argc);
            if (argc <= 0) {
                build_vc_demo(&cmd, &procs, name);
                if (!procs_wait_and_reset(&procs)) return 1;
                const char *src_path = temp_sprintf("./build/demos/%s.wasm", name);
                const char *dst_path = temp_sprintf("./wasm/%s.wasm", name);
                if (!copy_file(src_path, dst_path)) return 1;
                return 0;
            }

            const char *platform = shift(argv, argc);
            if (strcmp(platform, "sdl") == 0) {
                build_sdl_demo(&cmd, &procs, name);
                if (!procs_wait_and_reset(&procs)) return 1;
                if (argc <= 0) return 0;
                const char *run = shift(argv, argc);
                if (strcmp(run, "run") != 0) {
                    usage(program);
                    nob_log(ERROR, "unknown action `%s` for SDL demo: %s", run, name);
                    return 1;
                }
                cmd_append(&cmd, temp_sprintf("./build/demos/%s.sdl", name));
                if (!cmd_run_sync_and_reset(&cmd)) return 1;
                return 0;
            } else if (strcmp(platform, "term") == 0) {
                build_term_demo(&cmd, &procs, name);
                if (!procs_wait_and_reset(&procs)) return 1;
                if (argc <= 0) return 0;
                const char *run = shift(argv, argc);
                if (strcmp(run, "run") != 0) {
                    usage(program);
                    nob_log(ERROR, "unknown action `%s` for Terminal demo: %s", run, name);
                    return 1;
                }
                cmd_append(&cmd, temp_sprintf("./build/demos/%s.term", name));
                if (!cmd_run_sync_and_reset(&cmd)) return 1;
                return 0;
            } else if (strcmp(platform, "wasm") == 0) {
                build_wasm_demo(&cmd, &procs, name);
                if (!procs_wait_and_reset(&procs)) return 1;
                const char *src_path = temp_sprintf("./build/demos/%s.wasm", name);
                const char *dst_path = temp_sprintf("./wasm/%s.wasm", name);
                if (!copy_file(src_path, dst_path)) return 1;
                return 0;
            } else {
                usage(program);
                nob_log(ERROR, "unknown demo platform %s", platform);
                return 1;
            }
        } else if(strcmp(subcmd, "help") == 0) {
            usage(program);
            return 0;
        } else {
            usage(program);
            nob_log(ERROR, "Unknown command `%s`", subcmd);
            return 1;
        }
    } else {
        if (!build_tools(&cmd)) return 1;
        if (!build_assets(&cmd)) return 1;
        if (!build_tests(&cmd)) return 1;
        if (!build_all_vc_demos(&cmd, &procs)) return 1;
    }

    return 0;
}
