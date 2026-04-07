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

#ifndef LIBKRAKEN_KRAKEN_LOG_IMPL_H
#define LIBKRAKEN_KRAKEN_LOG_IMPL_H

#include "kraken_alloc.h"
#include "kraken_log.h"
#include "util/kraken_internal.h"
#include "util/kraken_string.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

#define KRAKEN_DEFINE_LOG_FN(L, l)                                                                                     \
    static void kraken_log_##l(const char* fmt, ...) {                                                                 \
        va_list args;                                                                                                  \
        va_start(args, fmt);                                                                                           \
        char* formatted_message = string_format_v(fmt, args);                                                          \
        va_end(args);                                                                                                  \
        kraken_log(KRAKEN_LOG_LEVEL_##L, formatted_message);                                                           \
        kraken_free(formatted_message);                                                                                \
    }

KRAKEN_API_BEGIN

void kraken_log(kraken_log_level_t level, const char* message);

#ifdef KRAKEN_DEBUG
KRAKEN_DEFINE_LOG_FN(DEBUG, debug)
#else
static void kraken_log_debug(const char* fmt, ...) {
}
#endif

KRAKEN_DEFINE_LOG_FN(ERROR, error)// Used mainly for panics

KRAKEN_API_END

#pragma clang diagnostic pop

#endif//LIBKRAKEN_KRAKEN_LOG_IMPL_H