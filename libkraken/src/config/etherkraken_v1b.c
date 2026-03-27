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
#include "kraken_io.h"
#include "kraken_io_impl.h"

constexpr uint32_t GPIO_REGISTER_SIZE = 32;
constexpr uint32_t GPIO_FSEL_BANK_SIZE = 10;

// clang-format off

static void v1b_gpio_state_update(void* base_address, void* shadow_memory,
                                             const kraken_io_c_handle_t* ios, size_t io_count) {
    volatile bcm2835_gpio_t* gpio = base_address;
    // Capture input state at the start of the update
    const uint32_t input_state_mask[] = {
        gpio->gplev0.value,
        gpio->gplev1.value
    };

    bcm2835_gpio_t* shadow_gpio = shadow_memory;
    // Compose the output state masks for the current io state
    uint32_t output_state_mask[2];
    for(size_t io_index = 0; io_index < io_count; io_index++) {
        kraken_io_t* io = (kraken_io_t*)ios[io_index];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const uint32_t bank = bcm_pin / GPIO_REGISTER_SIZE;
        const uint32_t bit = bcm_pin % GPIO_REGISTER_SIZE;
        if(io->pin_config.device_pin != bcm_pin) {
            continue;
        }
        switch(io->mode) {
            case KRAKEN_IO_MODE_IN: {
                io->state = (kraken_bool_t)(input_state_mask[bank] & 0b1 << bit);
                break;
            }
            case KRAKEN_IO_MODE_OUT: {
                output_state_mask[bank] |= ((uint8_t)io->state & 0b1) << bit;
                break;
            }
        }
        break;
    }
    // Calculate the toggle mask using the shadow state
    const uint32_t toggle0 = shadow_gpio->gplev0.value ^ output_state_mask[0];
    const uint32_t toggle1 = shadow_gpio->gplev1.value ^ output_state_mask[1];
    // Update the IO banks; first set bits, then clear bits
    gpio->gpset0.value = toggle0 & ~shadow_gpio->gplev0.value;
    gpio->gpclr0.value = ~toggle0 & shadow_gpio->gplev0.value;
    gpio->gpset1.value = toggle1 & ~shadow_gpio->gplev1.value;
    gpio->gpclr1.value = ~toggle1 & shadow_gpio->gplev1.value;
    // Update level shadow state by copying state mask into contiguous registers
    memcpy(&shadow_gpio->gplev0, output_state_mask, (GPIO_REGISTER_SIZE >> 3) << 1);
}

static void v1b_gpio_state_init(void* base_address, void* shadow_memory,
                                             const kraken_io_c_handle_t* ios, size_t io_count) {
    volatile bcm2835_gpio_t* gpio = base_address;
    memset(shadow_memory, 0x00, sizeof(bcm2835_gpio_t));
    // Build function selection mask for all IOs
    uint32_t fsel_mask[2];
    uint32_t output_mask[2];
    for(size_t i = 0; i < io_count; i++) {
        const kraken_io_t* io = (const kraken_io_t*)ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const uint32_t fsel_bank = bcm_pin / GPIO_FSEL_BANK_SIZE;
        const uint32_t fsel_bit = bcm_pin % GPIO_FSEL_BANK_SIZE * 3;
        fsel_mask[fsel_bank] |= (io->mode & 0b111) << fsel_bit;
        if(io->mode != KRAKEN_IO_MODE_OUT) {
            continue;
        }
        const uint32_t output_bank = bcm_pin / GPIO_REGISTER_SIZE;
        const uint32_t output_bit = bcm_pin % GPIO_REGISTER_SIZE;
        output_mask[output_bank] |= 0b1 << output_bit;
    }
    // Update IO functions
    gpio->gpfsel0.value = fsel_mask[0];
    gpio->gpfsel1.value = fsel_mask[1];
    // Clear all outputs to their default state
    gpio->gpclr0.value = output_mask[0];
    gpio->gpclr1.value = output_mask[1];
}

static void v1b_gpio_state_set(void* base_address, const kraken_bool_t state, const kraken_pin_config_t* pin) {
    bcm2835_gpio_t* gpio = base_address;
}

static kraken_bool_t v1b_i2c_mux_state_get(int fd, const kraken_pin_config_t* pin) {
    // TODO: implement I2C dance for getting state
    return KRAKEN_FALSE;
}

static void v1b_i2c_mux_state_set(int fd, const kraken_bool_t state, const kraken_pin_config_t* pin) {
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
            .pfn_state_get = &v1b_i2c_mux_state_get,
            .pfn_state_set = &v1b_i2c_mux_state_set
        }
    },
    { // IO1
        .i2c = {
            .type = KRAKEN_MUX_TYPE_I2C,
            .bus = "/dev/i2c-1",
            .address = 0x21,
            .pins = g_v1b_mux_pins,
            .pin_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_pins),
            .pfn_state_get = &v1b_i2c_mux_state_get,
            .pfn_state_set = &v1b_i2c_mux_state_set
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
        .pfn_state_update = &v1b_gpio_state_update,
        .pfn_state_init = &v1b_gpio_state_init
    },
    .mux_configs = g_v1b_mux_configs,
    .mux_count = KRAKEN_ARRAY_SIZE(g_v1b_mux_configs),
    .flash_device = "/dev/mtd0",
    .aux_pwr_en_pin = BCM2835_PIN_BCM27 // Pin 13 on daughterboard header
};

// clang-format on