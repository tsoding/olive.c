@echo off
rem launch this from msvs-enabled console

if not exist .\bin mkdir .\bin
if not exist .\bin\SDL2.dll if exist .\thirdparty\SDL2\lib\x64\SDL2.dll (
    copy .\thirdparty\SDL2\lib\x64\SDL2.dll .\bin\SDL2.dll
) else (
    echo Can't find SDL2.dll in thirdparty folder
)

set CFLAGS=/W4 /WX /std:c11 /FC /TC /Zi /wd4996 /wd4457 /wd4244 /wd4026 /nologo
set LIBS=thirdparty\SDL2\lib\x64\SDL2.lib ^
         thirdparty\SDL2\lib\x64\SDL2main.lib ^
         Shell32.lib

cl.exe %CFLAGS% /Fo.\bin\ /Fe.\bin\ test.c /I .\thirdparty
cl.exe %CFLAGS% /Fo.\bin\ /Fe.\bin\ examples/gallery.c /I .\thirdparty /I .
rem No WASM build.
cl.exe /O2 %CFLAGS% /DSDL_PLATFORM /Fo.\bin\ /Fe.\bin\ examples/triangle.c /I .\thirdparty /I . /link %LIBS% -SUBSYSTEM:windows
