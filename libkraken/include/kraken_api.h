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
#define KRAKEN_API_BEGIN extern "C" {
#define KRAKEN_API_END }
#else
#define KRAKEN_API_BEGIN
#define KRAKEN_API_END
#endif

#define KRAKEN_EXPORT __attribute__((visibility("default")))

typedef unsigned _BitInt(7) kraken_i2c_address_t;// I²C addresses are 7-bit

typedef enum kraken_bool : uint8_t {
    KRAKEN_TRUE,
    KRAKEN_FALSE
} kraken_bool_t;

#endif//LIBKRAKEN_KRAKEN_API_H