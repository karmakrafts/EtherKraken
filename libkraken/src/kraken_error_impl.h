#ifndef LIBKRAKEN_KRAKEN_ERROR_IMPL_H
#define LIBKRAKEN_KRAKEN_ERROR_IMPL_H

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void kraken_last_error_set(const char* error);

#ifdef KRAKEN_DEBUG
#define KRAKEN_CHECK(c, E, m)                                                                                          \
    do {                                                                                                               \
        if(!(c)) {                                                                                                     \
            kraken_last_error_set(m);                                                                                  \
            return E;                                                                                                  \
        }                                                                                                              \
    } while(0)

#define KRAKEN_CHECK_RESULT(r, E, m)                                                                                   \
    do {                                                                                                               \
        if(r != 0) {                                                                                                   \
            kraken_last_error_set(m);                                                                                  \
            return E;                                                                                                  \
        }                                                                                                              \
    } while(0)

#define KRAKEN_CHECK_ERROR(e, m)                                                                                       \
    do {                                                                                                               \
        const kraken_error_t __result = e;                                                                             \
        if(__result != KRAKEN_OK) {                                                                                    \
            kraken_last_error_set(m);                                                                                  \
            return __result;                                                                                           \
        }                                                                                                              \
    } while(0)

#define KRAKEN_CHECK_PTR(p, E, m)                                                                                      \
    do {                                                                                                               \
        if(p == nullptr) {                                                                                             \
            kraken_last_error_set(m);                                                                                  \
            return E;                                                                                                  \
        }                                                                                                              \
    } while(0)
#else
#define KRAKEN_CHECK(c, E, m)
#define KRAKEN_CHECK_RESULT(r, E, m)
#define KRAKEN_CHECK_PTR(p, E, m)
#define KRAKEN_CHECK_ERROR(e, m)
#endif

static _Noreturn void kraken_panic(const char* fmt, ...) {
    char* formatted_message = nullptr;
    va_list args;
    va_start(args);
    if(vasprintf(&formatted_message, fmt, args) == -1) {
        va_end(args);
        exit(-1);
        __builtin_unreachable();
    }
    va_end(args);
    printf("%s\n", formatted_message);
    free(formatted_message);
    __builtin_unreachable();
}

#endif//LIBKRAKEN_KRAKEN_ERROR_IMPL_H