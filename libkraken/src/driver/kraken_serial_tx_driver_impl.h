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

#ifndef LIBKRAKEN_KRAKEN_SERIAL_TX_DRIVER_IMPL_H
#define LIBKRAKEN_KRAKEN_SERIAL_TX_DRIVER_IMPL_H

#include "driver/kraken_serial_tx_driver.h"
#include "util/kraken_ms_queue.h"

KRAKEN_API_BEGIN

constexpr int8_t KRAKEN_SERIAL_TX_STOP_BIT = -1;

typedef struct kraken_serial_tx_state {
    kraken_serial_tx_config_t* config;
    kraken_io_t* clock_io;
    kraken_io_t* data_io;
    uint64_t io_mask;
    kraken_ms_queue_t buffer;
    uint8_t byte;   ///< Current byte in transmission
    int8_t bit;     ///< Current bit in transmission
    int8_t last_bit;///< Last bit of each word, determined by config word_size
} kraken_serial_tx_state_t;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_SERIAL_TX_DRIVER_IMPL_H