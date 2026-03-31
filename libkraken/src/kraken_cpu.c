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

KRAKEN_EXPORT kraken_error_t kraken_cpu_get_system_frequency(uint64_t* frequency) {
    KRAKEN_CHECK_PTR(frequency, KRAKEN_ERR_INVALID_ARG, "Invalid frequency pointer");
    volatile uint64_t result = 0;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(result));
    *frequency = result;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_cpu_get_counter(uint64_t* counter) {
    KRAKEN_CHECK_PTR(counter, KRAKEN_ERR_INVALID_ARG, "Invalid counter pointer");
    volatile uint64_t result = 0;
    asm volatile("mrs %0, cntvct_el0" : "=r"(result));
    *counter = result;
    return KRAKEN_OK;
}

KRAKEN_EXPORT void kraken_cpu_sleep_cycles(const uint64_t cycles) {
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