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

#ifndef LIBKRAKEN_KRAKEN_CONFIG_H
#define LIBKRAKEN_KRAKEN_CONFIG_H

#include "kraken_api.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

typedef struct kraken_pin_config {
    uint32_t device_pin;// Pin on the actual IC
    uint32_t port_pin;  // Pin on the port header on the board
} kraken_pin_config_t;

typedef void (*pfn_kraken_gpio_state_update)(void* base_address, void* shadow_memory, const kraken_io_c_handle_t* ios,
                                             size_t io_count);

typedef void (*pfn_kraken_gpio_state_init)(void* base_address, void* shadow_memory, const kraken_io_c_handle_t* ios,
                                           size_t io_count);

typedef struct kraken_gpio_config {
    const char* device_tree_entry;  // /proc/device-tree/..
    const char* device_type;        // bcm2835 etc. used for compatibility checking
    const char* device;             // /dev/..
    size_t registers_size;          // Actual size of the register region in bytes
    size_t mapped_size;             // Size of the mapped GPIO region in bytes, usually rounded to pages
    const kraken_pin_config_t* pins;// A pointer to all pin configurations for the GPIO device
    size_t pin_count;               // The number of pins as specified by the pointer above
    pfn_kraken_gpio_state_update pfn_state_update;
    pfn_kraken_gpio_state_init pfn_state_init;
} kraken_gpio_config_t;

typedef enum kraken_mux_type : uint8_t {
    KRAKEN_MUX_TYPE_I2C,
    KRAKEN_MUX_TYPE_SPI
} kraken_mux_type_t;

typedef void (*pfn_kraken_i2c_mux_state_update)(int fd, const kraken_io_c_handle_t* ios, size_t io_count);

typedef void (*pfn_kraken_i2c_mux_state_init)(int fd, const kraken_io_c_handle_t* ios, size_t io_count);

typedef struct kraken_i2c_mux_config {
    kraken_mux_type_t type;
    const kraken_pin_config_t* pins;
    size_t pin_count;
    const char* bus;
    kraken_i2c_address_t address;
    pfn_kraken_i2c_mux_state_update pfn_state_update;
    pfn_kraken_i2c_mux_state_init pfn_state_init;
} kraken_i2c_mux_config_t;

typedef void (*pfn_kraken_spi_mux_state_update)(void* base_address, void* shadow_memory,
                                                const kraken_io_c_handle_t* ios, size_t io_count);

typedef void (*pfn_kraken_spi_mux_state_init)(void* base_address, void* shadow_memory, const kraken_io_c_handle_t* ios,
                                              size_t io_count);

typedef struct kraken_spi_mux_config {
    kraken_mux_type_t type;
    const kraken_pin_config_t* pins;
    size_t pin_count;
    const char* device;
    pfn_kraken_spi_mux_state_update pfn_state_get;
    pfn_kraken_spi_mux_state_init pfn_state_set;
} kraken_spi_mux_config_t;

typedef struct kraken_mux_config {
    union {
        kraken_mux_type_t type;
        kraken_i2c_mux_config_t i2c;
        kraken_spi_mux_config_t spi;
    };
} kraken_mux_config_t;

typedef struct kraken_board_config {
    kraken_gpio_config_t gpio_config;
    const kraken_mux_config_t* mux_configs;
    size_t mux_count;
    const char* flash_device;
    uint32_t aux_pwr_en_pin;// BCM associated pin number
} kraken_board_config_t;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_CONFIG_H