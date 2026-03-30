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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include "kraken_error_impl.h"

#define KRAKEN_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define KRAKEN_INTERNAL __attribute__((visibility("hidden")))

#define kraken_alloc(t) (t*) malloc(sizeof(t))
#define kraken_alloc_array(t, s) (t*) malloc(sizeof(t) * (s))

#define kraken_assert(x)                                                                                               \
    do {                                                                                                               \
        if(!(x)) {                                                                                                     \
            kraken_panic("Assertion failed for expression %s", #x);                                                    \
        }                                                                                                              \
    } while(0)

static uint64_t get_system_counter_freq() {
    volatile uint64_t freq = 0;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

static void sleep_cycles(uint64_t cycles) {
    asm volatile(// clang-format off
        "isb\n" // Ensure all previous instructions completed
        "mrs x0, cntvct_el0\n"
        "add x1, x0, %0\n"
        "1:\n"
        "isb\n" // Prevent speculative execution inside the loop
        "mrs x2, cntvct_el0\n"
        "cmp x2, x1\n"
        "b.lt 1b\n"
        :
        : "r"(cycles)
        : "x0", "x1", "x2", "memory"
    );// clang-format on
}

#endif//LIBKRAKEN_KRAKEN_INTERNAL_H
