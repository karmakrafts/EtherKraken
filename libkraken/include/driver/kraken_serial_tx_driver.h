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

#ifndef LIBKRAKEN_KRAKEN_SERIAL_DRIVER_H
#define LIBKRAKEN_KRAKEN_SERIAL_DRIVER_H

#include "kraken_driver.h"

KRAKEN_API_BEGIN

typedef struct kraken_serial_tx_config {
    uint32_t clock_pin;
    uint32_t data_pin;
    int8_t word_size;// The size of each serial word in bits
} kraken_serial_tx_config_t;

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_create(const kraken_serial_tx_config_t* config,
                                                            kraken_port_handle_t port, kraken_driver_handle_t* handle);

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_write(kraken_driver_handle_t handle, const void* words,
                                                           size_t word_count);

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_destroy(kraken_driver_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_SERIAL_DRIVER_H