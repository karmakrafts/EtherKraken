cmake_minimum_required(VERSION 3.18)
project(libkraken LANGUAGES C)

set(CMAKE_C_STANDARD 23)

file(GLOB_RECURSE LK_SOURCE_FILES "src/*.c")
file(GLOB_RECURSE LK_PRV_HEADER_FILES "src/*.h")
file(GLOB_RECURSE LK_PUB_HEADER_FILES "include/*.h")

add_library(kraken SHARED
        "${LK_SOURCE_FILES}"
        "${LK_PRV_HEADER_FILES}"
        "${LK_PUB_HEADER_FILES}")
target_include_directories(kraken PRIVATE "src/")
target_include_directories(kraken PUBLIC "include/")
target_compile_definitions(kraken PUBLIC _GNU_SOURCE=1) # Enable pthread GNU extensions
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(kraken PUBLIC KRAKEN_DEBUG=1)
    target_compile_options(kraken PUBLIC -g -O0 -Wall)
else ()
    target_compile_options(kraken PUBLIC -O2)
endif ()

add_library(kraken-static STATIC
        "${LK_SOURCE_FILES}"
        "${LK_PRV_HEADER_FILES}"
        "${LK_PUB_HEADER_FILES}")
target_include_directories(kraken-static PRIVATE "src/")
target_include_directories(kraken-static PUBLIC "include/")
target_compile_definitions(kraken-static PUBLIC _GNU_SOURCE=1) # Enable pthread GNU extensions
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(kraken-static PUBLIC KRAKEN_DEBUG=1)
    target_compile_options(kraken-static PUBLIC -g -O0 -Wall)
else ()
    target_compile_options(kraken-static PUBLIC -O2)
endif ()