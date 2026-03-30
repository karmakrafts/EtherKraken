// Copyright 2026 Karma Krafts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LIBKRAKEN_KRAKEN_ERROR_IMPL_H
#define LIBKRAKEN_KRAKEN_ERROR_IMPL_H

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "kraken_error.h"
#include "kraken_log_impl.h"

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

#define kraken_panic(fmt, ...)                                                                                         \
    do {                                                                                                               \
        char* formatted_message = kraken_format("!HAL PANIC! " fmt, ##__VA_ARGS__);                                    \
        kraken_log_error(formatted_message);                                                                           \
        kraken_free(formatted_message);                                                                                \
        __builtin_trap();                                                                                              \
    } while(0)

#endif//LIBKRAKEN_KRAKEN_ERROR_IMPL_H