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

#ifndef LIBKRAKEN_KRAKEN_I2C_MUX_PORT_H
#define LIBKRAKEN_KRAKEN_I2C_MUX_PORT_H

#include "kraken_config.h"
#include "kraken_port.h"

typedef struct kraken_i2c_mux_port {
    kraken_port_type_t type;
    int fd;
    kraken_i2c_mux_config_t config;
    uint32_t index;
    kraken_i2c_address_t address;
} kraken_i2c_mux_port_t;

kraken_error_t kraken_i2c_mux_port_create(kraken_i2c_mux_port_t** port_addr, uint32_t index,
                                          const kraken_i2c_mux_config_t* config);

kraken_error_t kraken_i2c_mux_port_destroy(kraken_i2c_mux_port_t* port);

#endif//LIBKRAKEN_KRAKEN_I2C_MUX_PORT_H