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

#ifndef LIBKRAKEN_KRAKEN_API_H
#define LIBKRAKEN_KRAKEN_API_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
/// @brief Marks the beginning of C API declarations.
#define KRAKEN_API_BEGIN extern "C" {
/// @brief Marks the end of C API declarations.
#define KRAKEN_API_END }
#else
/// @brief Marks the beginning of C API declarations.
#define KRAKEN_API_BEGIN
/// @brief Marks the end of C API declarations.
#define KRAKEN_API_END
#endif

/// @brief Marks a function as exported from the library.
#define KRAKEN_EXPORT __attribute__((visibility("default")))

/// @brief I2C device address type.
typedef uint8_t kraken_i2c_address_t;

/// @brief Boolean type for use in the Kraken API.
typedef enum kraken_bool : uint8_t {
    KRAKEN_FALSE,///< Logical false.
    KRAKEN_TRUE  ///< Logical true.
} kraken_bool_t;

#endif//LIBKRAKEN_KRAKEN_API_H