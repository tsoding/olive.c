cmake_minimum_required(VERSION 3.18.2)
project(olivest LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

add_library(olivest SHARED
        main.c
        ../../../../../olive.c
)

find_library(jnigraphics-lib jnigraphics)
find_library(android-lib android)
find_library(log-lib log)

target_link_libraries(olivest
        ${jnigraphics-lib}
        ${android-lib}
        ${log-lib}
)