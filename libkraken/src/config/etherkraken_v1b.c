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
#include "device/mcp23017.h"
#include "kraken_io.h"
#include "util/kraken_internal.h"

// clang-format off

static kraken_pin_config_t g_v1b_mux_pins[] = {
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

static kraken_mux_config_t g_v1b_mux_config_io0 = {
    .spi = {
        .type = KRAKEN_MUX_TYPE_SPI,
        .pins = g_v1b_mux_pins,
        .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_pins),
        .pfn_state_update = &mcp23017_spi_mux_state_update,
        .pfn_state_init = &mcp23017_spi_mux_state_init
    }
};
static kraken_mux_config_t g_v1b_mux_config_io1 = {
    .spi = {
        .type = KRAKEN_MUX_TYPE_SPI,
        .pins = g_v1b_mux_pins,
        .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_pins),
        .pfn_state_update = &mcp23017_spi_mux_state_update,
        .pfn_state_init = &mcp23017_spi_mux_state_init
    }
};

static kraken_mux_config_t* g_v1b_mux_configs[] = {
    &g_v1b_mux_config_io0,
    &g_v1b_mux_config_io1
};

// On the V1B, only 8 GPIOS from the SoC are actually programmable
static kraken_pin_config_t g_v1b_gpio_pins[] = {
    // CTR lines on the EXT port
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
    },
    // Board-internal lines
    { // AUX power state
        .device_pin = BCM2835_PIN_BCM27,
        .port_pin = 0
    },
    { // LED
        .device_pin = BCM2835_PIN_BCM47,
        .port_pin = 0
    }
};

static kraken_gpio_config_t g_v1b_gpio_config = {
    .device_tree_entry = "/proc/device-tree/soc/gpiomem",
    .device_type = "bcm2835",
    .device = "/dev/gpiomem",
    .registers_size = sizeof(bcm2835_gpio_t),
    .pins = g_v1b_gpio_pins,
    .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_gpio_pins),
    .pfn_state_update = &bcm2835_gpio_state_update,
    .pfn_state_init = &bcm2835_gpio_state_init
};

const kraken_board_config_t g_config_v1b = {
    .gpio_config = &g_v1b_gpio_config,
    .mux_configs = g_v1b_mux_configs,
    .mux_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_configs),
    .flash_device = "/dev/mtd0"
};

// clang-format on