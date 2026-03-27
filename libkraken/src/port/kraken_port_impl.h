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

#ifndef LIBKRAKEN_KRAKEN_PORT_IMPL_H
#define LIBKRAKEN_KRAKEN_PORT_IMPL_H

#include "kraken_gpio_port.h"
#include "kraken_i2c_mux_port.h"
#include "kraken_port.h"
#include "kraken_spi_mux_port.h"

typedef struct kraken_port {
    union {
        kraken_port_type_t type;
        kraken_gpio_port_t gpio;
        kraken_i2c_mux_port_t i2c_mux;
        kraken_spi_mux_port_t spi_mux;
    };
} kraken_port_t;

#endif//LIBKRAKEN_KRAKEN_PORT_IMPL_H