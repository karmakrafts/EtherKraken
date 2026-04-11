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

#ifndef LIBKRAKEN_KRAKEN_UART_H
#define LIBKRAKEN_KRAKEN_UART_H

#include "kraken_api.h"

KRAKEN_API_BEGIN

typedef enum kraken_uart_parity : uint8_t {
    KRAKEN_UART_PARITY_NONE,///< No parity bit is sent over the wire
    KRAKEN_UART_PARITY_ODD,///< The parity bit is set so that the total number of 1s in the data bits plus the parity bit is odd
    KRAKEN_UART_PARITY_EVEN,///< The parity bit is set so that the total number of 1s in the data bits plus the parity bit is even
    KRAKEN_UART_PARITY_MARK,///< The parity bit is always set to 1
    KRAKEN_UART_PARITY_SPACE///< The parity bit is always set to 0
} kraken_uart_parity_t;

typedef struct kraken_uart_config {
    uint32_t pin;               ///< Pin-number on the device
    uint32_t buffer_size;       ///< Data buffer size in bytes
    int8_t data_bits;           ///< Number of data bits
    int8_t stop_bits;           ///< Number of stop bits
    kraken_uart_parity_t parity;///< Type of parity
} kraken_uart_config_t;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_UART_H