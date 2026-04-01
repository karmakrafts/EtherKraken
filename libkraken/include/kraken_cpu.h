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

#ifndef LIBKRAKEN_KRAKEN_CPU_H
#define LIBKRAKEN_KRAKEN_CPU_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

/// @brief Retrieves the current system CPU counter frequency.
///
/// @param[out] frequency Pointer to store the current system frequency.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_cpu_get_system_frequency(register uint64_t* frequency);

/// @brief Retrieves the current system CPU counter.
///
/// @param[out] counter Pointer to store the current system counter value.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_cpu_get_counter(register uint64_t* counter);

/// @brief Busy-waits for a specified number of CPU cycles.
///
/// @param[in] cycles The number of CPU cycles to sleep.
KRAKEN_EXPORT void kraken_cpu_sleep_cycles(register uint64_t cycles);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_CPU_H