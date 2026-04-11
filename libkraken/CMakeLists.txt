cmake_minimum_required(VERSION 3.18)
project(libkraken LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

# AtomicQueue

FetchContent_Declare(
        atomic_queue
        GIT_REPOSITORY https://github.com/max0x7ba/atomic_queue.git
        GIT_TAG "v1.7.3")
FetchContent_MakeAvailable(atomic_queue)

# kraken/kraken-static targets

file(GLOB_RECURSE LK_SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.c*")
file(GLOB_RECURSE LK_PRV_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/*.h*")
file(GLOB_RECURSE LK_PUB_HEADER_FILES "${CMAKE_SOURCE_DIR}/include/*.h")

add_library(kraken SHARED
        "${LK_SOURCE_FILES}"
        "${LK_PRV_HEADER_FILES}"
        "${LK_PUB_HEADER_FILES}")
target_include_directories(kraken PRIVATE "${CMAKE_SOURCE_DIR}/src/")
target_include_directories(kraken PUBLIC "${CMAKE_SOURCE_DIR}/include/")
target_compile_definitions(kraken PUBLIC _GNU_SOURCE=1) # Enable pthread GNU extensions
target_link_libraries(kraken PUBLIC atomic_queue::atomic_queue)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(kraken PUBLIC KRAKEN_DEBUG=1)
    target_compile_options(kraken PUBLIC -g -O0)
else ()
    target_compile_options(kraken PUBLIC -O3)
endif ()

add_library(kraken-static STATIC
        "${LK_SOURCE_FILES}"
        "${LK_PRV_HEADER_FILES}"
        "${LK_PUB_HEADER_FILES}")
target_include_directories(kraken-static PRIVATE "${CMAKE_SOURCE_DIR}/src/")
target_include_directories(kraken-static PUBLIC "${CMAKE_SOURCE_DIR}/include/")
target_compile_definitions(kraken-static PUBLIC _GNU_SOURCE=1) # Enable pthread GNU extensions
target_link_libraries(kraken-static PUBLIC atomic_queue::atomic_queue)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(kraken-static PUBLIC KRAKEN_DEBUG=1)
    target_compile_options(kraken-static PUBLIC -g -O0)
else ()
    target_compile_options(kraken-static PUBLIC -O3)
endif ()

# kraken-tests target

if (KRAKEN_HOST_TESTS)
    message(STATUS "Enabled host tests")

    target_compile_definitions(kraken PUBLIC KRAKEN_TEST_ENV=1)
    target_compile_definitions(kraken-static PUBLIC KRAKEN_TEST_ENV=1)

    set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            google-test
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG "v1.17.0")
    FetchContent_MakeAvailable(google-test)
    enable_testing(TRUE)
    include(GoogleTest)

    file(GLOB_RECURSE LK_TEST_SOURCE_FILES "${CMAKE_SOURCE_DIR}/test/*.c*")
    file(GLOB_RECURSE LK_TEST_HEADER_FILES "${CMAKE_SOURCE_DIR}/test/*.h*")
    add_executable(kraken-tests
            "${LK_SOURCE_FILES}"
            "${LK_PRV_HEADER_FILES}"
            "${LK_PUB_HEADER_FILES}"
            "${LK_TEST_SOURCE_FILES}"
            "${LK_TEST_HEADER_FILES}")
    target_compile_definitions(kraken-tests PUBLIC KRAKEN_TEST_ENV=1 _GNU_SOURCE=1)
    target_include_directories(kraken-tests PRIVATE
            "${CMAKE_SOURCE_DIR}/src/"
            "${CMAKE_SOURCE_DIR}/include/"
            "${CMAKE_SOURCE_DIR}/test/")
    gtest_discover_tests(kraken-tests)
    target_link_libraries(kraken-tests PUBLIC gtest_main)
endif ()