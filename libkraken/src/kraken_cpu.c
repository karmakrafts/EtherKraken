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

#include "kraken_cpu.h"
#include "kraken_error_impl.h"

KRAKEN_EXPORT KRAKEN_NOINLINE KRAKEN_NOOPT kraken_error_t
kraken_cpu_get_system_frequency(register uint64_t* frequency) {
    KRAKEN_CHECK_PTR(frequency, KRAKEN_ERR_INVALID_ARG, "Invalid frequency pointer");
#ifdef KRAKEN_TEST_ENV
    uint64_t counter = 0;
    uint64_t counter_now = 0;
    kraken_cpu_get_counter(&counter);
    sleep(1);
    kraken_cpu_get_counter(&counter_now);
    *frequency = counter_now - counter;
#else
    volatile register uint64_t result = 0;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(result));
    *frequency = result;
#endif
    return KRAKEN_OK;
}

KRAKEN_EXPORT KRAKEN_NOINLINE KRAKEN_NOOPT kraken_error_t kraken_cpu_get_counter(register uint64_t* counter) {
    KRAKEN_CHECK_PTR(counter, KRAKEN_ERR_INVALID_ARG, "Invalid counter pointer");
#ifdef KRAKEN_TEST_ENV
    volatile register uint32_t lo;
    volatile register uint32_t hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    *counter = (uint64_t) hi << 32 | lo;
#else
    volatile register uint64_t result = 0;
    asm volatile("mrs %0, cntvct_el0" : "=r"(result));
    *counter = result;
#endif
    return KRAKEN_OK;
}

KRAKEN_EXPORT KRAKEN_NOINLINE KRAKEN_NOOPT void kraken_cpu_sleep_cycles(const register uint64_t cycles) {
#ifdef KRAKEN_TEST_ENV
    uint64_t counter = 0;
    kraken_cpu_get_counter(&counter);
    const uint64_t target_counter = counter + cycles;
    while(counter < target_counter) {
        kraken_cpu_get_counter(&counter);
    }
#else
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
        : "memory"
    );// clang-format on
#endif
}