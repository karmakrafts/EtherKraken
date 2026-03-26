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

KRAKEN_API_BEGIN

typedef struct kraken_pin_config {
    uint32_t device_pin;
    uint32_t port_pin;
} kraken_pin_config_t;

typedef kraken_bool_t (*pfn_kraken_gpio_state_get)(const void* base_address);
typedef void (*pfn_kraken_gpio_state_set)(void* base_address, kraken_bool_t state);

typedef struct kraken_gpio_config {
    const char* device;
    size_t registers_size;
    size_t mapped_size;
    const kraken_pin_config_t* pins;
    size_t pin_count;
    pfn_kraken_gpio_state_get pfn_state_get;
    pfn_kraken_gpio_state_set pfn_state_set;
} kraken_gpio_config_t;

typedef kraken_bool_t (*pfn_kraken_mux_state_get)(int fd);
typedef void (*pfn_kraken_mux_state_set)(int fd, kraken_bool_t state);

typedef struct kraken_mux_config {
    const char* bus;
    kraken_mux_address_t address;
    const kraken_pin_config_t* pins;
    size_t pin_count;
    pfn_kraken_mux_state_get pfn_state_get;
    pfn_kraken_mux_state_set pfn_state_set;
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