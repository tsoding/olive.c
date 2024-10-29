#define NOB_IMPLEMENTATION
#include "nob.h"


#define COMMON_CFLAGS "-Wall", "-Wextra", "-pedantic", "-std=c11", "-ggdb", "-I.", "-I./build/", "-I./dev-deps/"



void build_tools(void)
{
    Nob_Cmd cmd = {0};
    if (!nob_mkdir_if_not_exists("build")){
        nob_log(NOB_ERROR, "Could not create build folder.\n");
        exit(1);
    }
    if (!nob_mkdir_if_not_exists("build/tools")){
        nob_log(NOB_ERROR, "Could not create build/tools folder.\n");
        exit(1);
    }
    nob_cmd_append(&cmd, "clang", COMMON_CFLAGS, "-o", "./build/tools/png2c", "./tools/png2c.c", "-lm");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "clang", COMMON_CFLAGS, "-o", "./build/tools/obj2c", "./tools/obj2c.c", "-lm");
    if (!nob_cmd_run_sync(cmd)) exit(1);
}

void build_assets(void)
{
    if (!nob_mkdir_if_not_exists("build")){
        nob_log(NOB_ERROR, "Could not create build folder.\n");
        exit(1);
    }
    if (!nob_mkdir_if_not_exists("build/assets")){
        nob_log(NOB_ERROR, "Could not create build/assets folder.\n");
        exit(1);
    }
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "./build/tools/png2c", "-n", "tsodinPog", "-o", "./build/assets/tsodinPog.c", "./assets/tsodinPog.png");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "./build/tools/png2c", "-n", "tsodinCup", "-o", "./build/assets/tsodinCup.c", "./assets/tsodinCup.png");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "./build/tools/png2c", "-n", "oldstone", "-o", "./build/assets/oldstone.c", "./assets/oldstone.png");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "./build/tools/png2c", "-n", "lavastone", "-o", "./build/assets/lavastone.c", "./assets/lavastone.png");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "./build/tools/obj2c", "-o", "./build/assets/tsodinCupLowPoly.c", "./assets/tsodinCupLowPoly.obj");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "./build/tools/obj2c", "-s", "0.40", "-o", "./build/assets/utahTeapot.c", "./assets/utahTeapot.obj");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
    nob_cmd_append(&cmd, "./build/tools/obj2c", "-s", "1.5", "-o", "./build/assets/penger.c", "./assets/penger_obj/penger.obj");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
}

void build_tests(void)
{
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "clang", COMMON_CFLAGS, "-fsanitize=memory", "-o", "./build/test", "test.c", "-lm");
    if (!nob_cmd_run_sync(cmd)) exit(1);
    cmd.count = 0;
}

Nob_Proc build_wasm_demo(const char *name)
{
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd,"clang", COMMON_CFLAGS, "-O2", "-fno-builtin", "--target=wasm32", "--no-standard-libraries", "-Wl,--no-entry", "-Wl,--export=vc_render", "-Wl,--export=__heap_base", "-Wl,--allow-undefined", "-o", nob_temp_sprintf("./build/demos/%s.wasm", name), "-DVC_PLATFORM=VC_WASM_PLATFORM", nob_temp_sprintf("./demos/%s.c", name));
    Nob_String_Builder render = {0};
    nob_cmd_render(cmd, &render);
    nob_sb_append_null(&render);
    nob_log(NOB_INFO, render.items);
    return nob_cmd_run_async(cmd);
}

Nob_Proc build_term_demo(const char *name)
{
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "clang", COMMON_CFLAGS, "-O2", "-o", nob_temp_sprintf("./build/demos/%s.term", name), "-DVC_PLATFORM=VC_TERM_PLATFORM", "-D_XOPEN_SOURCE=600", nob_temp_sprintf("./demos/%s.c", name), "-lm");
    Nob_String_Builder render = {0};
    nob_cmd_render(cmd, &render);
    nob_sb_append_null(&render);
    nob_log(NOB_INFO, render.items);
    return nob_cmd_run_async(cmd);
}

Nob_Proc build_sdl_demo(const char *name)
{
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "clang", COMMON_CFLAGS, "-O2", "-o", nob_temp_sprintf("./build/demos/%s.sdl", name), "-DVC_PLATFORM=VC_SDL_PLATFORM", nob_temp_sprintf("./demos/%s.c", name), "-lm", "-lSDL2", NULL);
    Nob_String_Builder render = {0};
    nob_cmd_render(cmd, &render);
    nob_sb_append_null(&render);
    nob_log(NOB_INFO, render.items);
    return nob_cmd_run_async(cmd);
}

void usage(const char *program)
{
    nob_log(NOB_INFO, "Usage: %s [<subcommand>]", program);
    nob_log(NOB_INFO, "Subcommands:");
    nob_log(NOB_INFO, "    tools");
    nob_log(NOB_INFO, "        Build all the tools. Things like png2c, obj2c, etc.");
    nob_log(NOB_INFO, "    assets");
    nob_log(NOB_INFO, "        Build the assets in the assets/ folder.");
    nob_log(NOB_INFO, "        Basically convert their data to C code so we can bake them in demos.");
    nob_log(NOB_INFO, "    test[s] [<args>]");
    nob_log(NOB_INFO, "        Build and run test.c");
    nob_log(NOB_INFO, "        If <args> are provided the test utility is run with them.");
    nob_log(NOB_INFO, "    demos [<platform>] [run]");
    nob_log(NOB_INFO, "        Build demos.");
    nob_log(NOB_INFO, "        Available platforms are: sdl, term, or wasm.");
    nob_log(NOB_INFO, "        Optional [run] runs the demo after the build.");
    nob_log(NOB_INFO, "        [run] is not available for wasm platform.");
    nob_log(NOB_INFO, "    help");
    nob_log(NOB_INFO, "         Print this message");
}


void build_vc_demo(const char *name, Nob_Procs *procs)
{
    nob_da_append(procs, build_wasm_demo(name));
    nob_da_append(procs, build_term_demo(name));
    nob_da_append(procs, build_sdl_demo(name));
}

void build_all_vc_demos(void)
{
    if (!nob_mkdir_if_not_exists("build")){
        nob_log(NOB_ERROR, "Could not create build folder.\n");
        exit(1);
    }

    if (!nob_mkdir_if_not_exists("build/demos")){
        nob_log(NOB_ERROR, "Could not create build/demos folder.\n");
        exit(1);
    }

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
    size_t names_sz = sizeof(names)/sizeof(names[0]);
    size_t thread_count = 6;

    Nob_Procs procs = {0};
    for (size_t i = 0; i < names_sz; ++i) {
        build_vc_demo(names[i], &procs);
        if (procs.count >= thread_count) {
            nob_procs_wait(procs);
            procs.count = 0;
        }
    }
    nob_procs_wait(procs);

    for (size_t i = 0; i < names_sz; ++i) {
        nob_copy_file(nob_temp_sprintf("./build/demos/%s.wasm", names[i]), nob_temp_sprintf("./wasm/%s.wasm", names[i]));
   }
}

int main(int argc, char** argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    const char *program = nob_shift_args(&argc, &argv);

    if (argc > 0) {
        const char *subcmd = nob_shift_args(&argc, &argv);
        if (strcmp(subcmd, "tools") == 0) {
            build_tools();
            return 1;
        } else if (strcmp(subcmd, "assets") == 0) {
            build_assets();
        } else if (strcmp(subcmd, "tests") == 0 || strcmp(subcmd, "test") == 0) {
            build_tests();
            if (argc > 0) {
                Nob_Cmd cmd = {0};
                nob_cmd_append(&cmd, "./build/test");
                for (int i = 0; i < argc; ++i) {
                    nob_cmd_append(&cmd, argv[i]);
                }
                if (!nob_cmd_run_sync(cmd)) exit(1);
            }
        } else if (strcmp(subcmd, "demos") == 0) {
            if (argc > 0) {
                const char *name = nob_shift_args(&argc, &argv);

                if (argc > 0) {
                    const char *platform = nob_shift_args(&argc, &argv);
                    if (strcmp(platform, "sdl") == 0) {
                        nob_proc_wait(build_sdl_demo(name));

                        if (argc > 0) {
                            const char *run = nob_shift_args(&argc, &argv);
                            if (strcmp(run, "run") != 0) {
                                usage(program);
                                nob_log(NOB_ERROR, "unknown action `%s` for SDL demo: %s", run, name);
                                exit(1);
                            }
                            Nob_Cmd cmd = {0};
                            nob_cmd_append(&cmd, nob_temp_sprintf("./build/demos/%s.sdl", name));
                        }
                    } else if (strcmp(platform, "term") == 0) {
                        nob_proc_wait(build_term_demo(name));

                        if (argc > 0) {
                            const char *run = nob_shift_args(&argc, &argv);
                            if (strcmp(run, "run") != 0) {
                                usage(program);
                                nob_log(NOB_ERROR, "unknown action `%s` for Terminal demo: %s", run, name);
                                exit(1);
                            }
                            Nob_Cmd cmd = {0};
                            nob_cmd_append(&cmd, nob_temp_sprintf("./build/demos/%s.term", name));
                        }
                    } else if (strcmp(platform, "wasm") == 0) {
                        nob_proc_wait(build_wasm_demo(name));
                        nob_copy_file(nob_temp_sprintf("./build/demos/%s.wasm", name), nob_temp_sprintf("./wasm/%s.wasm", name));
                    } else {
                        usage(program);
                        nob_log(NOB_ERROR, "unknown demo platform %s", platform);
                        exit(1);
                    }
                } else {
                    Nob_Procs procs = {0};
                    build_vc_demo(name, &procs);
                    nob_procs_wait(procs);
                    nob_copy_file(nob_temp_sprintf("./build/demos/", name, ".wasm"), nob_temp_sprintf("./wasm/%s.wasm", name));
                }
            } else {
                build_all_vc_demos();
            }
        } else if(strcmp(subcmd, "help") == 0) {
            usage(program);
        } else {
            usage(program);
            nob_log(NOB_ERROR, "Unknown command `%s`", subcmd);
            exit(1);
        }
    } else {
        build_tools();
        build_assets();
        build_tests();
        build_all_vc_demos();
    }

    return 0;
}
