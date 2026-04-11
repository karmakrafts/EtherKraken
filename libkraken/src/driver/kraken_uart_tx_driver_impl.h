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

#ifndef LIBKRAKEN_KRAKEN_UART_TX_DRIVER_IMPL_H
#define LIBKRAKEN_KRAKEN_UART_TX_DRIVER_IMPL_H

#include "driver/kraken_uart_tx_driver.h"
#include "kraken_io_impl.h"
#include "util/kraken_byte_queue.h"

KRAKEN_API_BEGIN

constexpr int8_t KRAKEN_UART_TX_STOP_BIT = -1;
constexpr int8_t KRAKEN_UART_TX_NO_PARITY = -1;

typedef struct kraken_uart_tx_state {
    kraken_uart_config_t* config;
    uint64_t io_mask;
    kraken_io_t* io;
    kraken_byte_queue_handle_t buffer;
    int8_t frame_size;

    int8_t start_bit;
    int8_t first_data_bit;
    int8_t last_data_bit;
    int8_t parity_bit; ///< Parity bit index, -1 if parity is KRAKEN_UART_PARITY_NONE
    int8_t first_stop_bit;
    int8_t last_stop_bit;
    _Atomic(int8_t) bit;

    _Atomic(uint8_t) byte;
} kraken_uart_tx_state_t;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_UART_TX_DRIVER_IMPL_H