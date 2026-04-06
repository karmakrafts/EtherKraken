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

#ifndef LIBKRAKEN_KRAKEN_SPI_MUX_PORT_H
#define LIBKRAKEN_KRAKEN_SPI_MUX_PORT_H

#include "config/kraken_config.h"
#include "kraken_io_impl.h"
#include "kraken_port.h"

KRAKEN_API_BEGIN

typedef struct kraken_spi_mux_port {
    kraken_port_type_t type;
    kraken_io_t** ios;
    size_t num_ios;
    int fd;
    kraken_spi_mux_config_t* config;
    void* registers;
    size_t mapped_registers_size;
    void* shadow_memory;
} kraken_spi_mux_port_t;

kraken_error_t kraken_spi_mux_port_create(kraken_spi_mux_port_t** port_addr, const kraken_spi_mux_config_t* config);

kraken_error_t kraken_spi_mux_port_destroy(kraken_spi_mux_port_t* port);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_SPI_MUX_PORT_H