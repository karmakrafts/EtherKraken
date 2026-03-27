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

#include "etherkraken_v1b.h"
#include "device/bcm2835.h"
#include "kraken_internal.h"

// clang-format off

static kraken_bool_t v1b_gpio_state_get(const void* base_address) {
    const bcm2835_gpio_t* gpio = base_address;
    return KRAKEN_FALSE;
}

static void v1b_gpio_state_set(void* base_address, const kraken_bool_t state) {
    bcm2835_gpio_t* gpio = base_address;
}

static kraken_bool_t v1b_mux_state_get(int fd) {
    // TODO: implement I2C dance for getting state
    return KRAKEN_FALSE;
}

static void v1b_mux_state_set(int fd, const kraken_bool_t state) {
    // TODO: implement I2C dance for setting state
}

static const kraken_pin_config_t g_v1b_mux_pins[] = {
    // Port A
    { .device_pin = 21, .port_pin = 3 },
    { .device_pin = 22, .port_pin = 5 },
    { .device_pin = 23, .port_pin = 7 },
    { .device_pin = 24, .port_pin = 9 },
    { .device_pin = 25, .port_pin = 11 },
    { .device_pin = 26, .port_pin = 13 },
    { .device_pin = 27, .port_pin = 15 },
    { .device_pin = 28, .port_pin = 17 },
    // Port B
    { .device_pin = 1, .port_pin = 4 },
    { .device_pin = 2, .port_pin = 6 },
    { .device_pin = 3, .port_pin = 8 },
    { .device_pin = 4, .port_pin = 10 },
    { .device_pin = 5, .port_pin = 12 },
    { .device_pin = 6, .port_pin = 14 },
    { .device_pin = 7, .port_pin = 16 },
    { .device_pin = 8, .port_pin = 18 }
};

// TODO: change to SPI muxes for on-board IOs when PCB is adjusted
static const kraken_mux_config_t g_v1b_mux_configs[] = {
    { // IO0
        .i2c = {
            .type = KRAKEN_MUX_TYPE_I2C,
            .bus = "/dev/i2c-1",
            .address = 0x20,
            .pins = g_v1b_mux_pins,
            .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_pins),
            .pfn_state_get = &v1b_mux_state_get,
            .pfn_state_set = &v1b_mux_state_set
        }
    },
    { // IO1
        .i2c = {
            .type = KRAKEN_MUX_TYPE_I2C,
            .bus = "/dev/i2c-1",
            .address = 0x21,
            .pins = g_v1b_mux_pins,
            .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_pins),
            .pfn_state_get = &v1b_mux_state_get,
            .pfn_state_set = &v1b_mux_state_set
        }
    }
};

// On the V1B, only 6 GPIOS from the SoC are actually programmable on the EXT connector
static const kraken_pin_config_t g_v1b_gpio_pins[] = {
    {
        .device_pin = BCM2835_PIN_BCM13,
        .port_pin = 8
    },
    {
        .device_pin = BCM2835_PIN_BCM25,
        .port_pin = 10
    },
    {
        .device_pin = BCM2835_PIN_BCM24,
        .port_pin = 12
    },
    {
        .device_pin = BCM2835_PIN_BCM23,
        .port_pin = 14
    },
    {
        .device_pin = BCM2835_PIN_BCM22,
        .port_pin = 16
    },
    {
        .device_pin = BCM2835_PIN_BCM26,
        .port_pin = 18
    }
};

const kraken_board_config_t g_config_v1b = {
    .gpio_config = {
        .device_tree_entry = "/proc/device-tree/soc/gpiomem",
        .device_type = "bcm2835",
        .device = "/dev/gpiomem",
        .mapped_size = 4096,
        .registers_size = sizeof(bcm2835_gpio_t),
        .pins = g_v1b_gpio_pins,
        .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_gpio_pins),
        .pfn_state_get = &v1b_gpio_state_get,
        .pfn_state_set = &v1b_gpio_state_set
    },
    .mux_configs = g_v1b_mux_configs,
    .mux_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_configs),
    .flash_device = "/dev/mtd0",
    .aux_pwr_en_pin = BCM2835_PIN_BCM27 // Pin 13 on daughterboard header
};

// clang-format on