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

#ifndef LIBKRAKEN_KRAKEN_INTERNAL_H
#define LIBKRAKEN_KRAKEN_INTERNAL_H

#include <malloc.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include "kraken_error_impl.h"

#define KRAKEN_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define KRAKEN_INTERNAL __attribute__((visibility("hidden")))
#define KRAKEN_INLINE __attribute__((always_inline))
#define KRAKEN_NOINLINE __attribute__((noinline))
#define KRAKEN_NOOPT __attribute__((optnone))

#define kraken_assert(x)                                                                                               \
    do {                                                                                                               \
        if(!(x)) {                                                                                                     \
            kraken_panic("Assertion failed for expression %s", #x);                                                    \
        }                                                                                                              \
    } while(0)

#undef thread_local// threads.h defines macro alias, but C23 has proper keyword
#undef sched_priority

#endif//LIBKRAKEN_KRAKEN_INTERNAL_H
