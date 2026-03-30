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

#include "../../include/device/bcm2835.h"

#include "kraken_internal.h"
#include "kraken_io_impl.h"
#include "kraken_log_impl.h"

constexpr uint32_t GPIO_REGISTER_SIZE = 32;
constexpr uint32_t GPIO_FSEL_BANK_SIZE = 10;

KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_update(void* base_address, void*, kraken_io_handle_t* ios,
                                                       const size_t io_count) {
    volatile bcm2835_gpio_t* gpio = base_address;
    kraken_log_debug("Performing BCM2835 GPIO state update");
    // Capture input state at the start of the update
    const uint32_t input_state_mask[2] = {gpio->gplev0.value, gpio->gplev1.value};
    kraken_log_debug("Read BCM2835 GPLEV0 register as 0x%08X", input_state_mask[0]);
    kraken_log_debug("Read BCM2835 GPLEV1 register as 0x%08X", input_state_mask[1]);

    // Compose the output state masks for the current io state
    uint32_t set_mask[2] = {0x00000000, 0x00000000};
    uint32_t clr_mask[2] = {0x00000000, 0x00000000};
    for(size_t io_index = 0; io_index < io_count; io_index++) {
        kraken_io_t* io = (kraken_io_t*) ios[io_index];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const uint32_t bank = bcm_pin / GPIO_REGISTER_SIZE;
        const uint32_t bit = bcm_pin % GPIO_REGISTER_SIZE;
        set_mask[bank] |= ((uint32_t) io->state & (uint32_t) io->mode & 0b1) << bit;
        clr_mask[bank] |= (~(uint32_t) io->state & (uint32_t) io->mode & 0b1) << bit;
        io->state = (kraken_bool_t) (input_state_mask[bank] & (~(uint32_t) io->mode & 0b1) << bit);
    }
    kraken_log_debug("Writing 0x%08X to BCM2835 GPCLR0 register", clr_mask[0]);
    gpio->gpclr0.value = clr_mask[0];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPSET0 register", set_mask[0]);
    gpio->gpset0.value = set_mask[0];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPCLR1 register", clr_mask[1]);
    gpio->gpclr1.value = clr_mask[1];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPSET1 register", set_mask[1]);
    gpio->gpset1.value = set_mask[1];

    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_init(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                     const size_t io_count) {
    volatile bcm2835_gpio_t* gpio = base_address;
    kraken_log_debug("Performing BCM2835 GPIO initialization");
    memset(shadow_memory, 0x00, sizeof(bcm2835_gpio_t));
    // Build function selection mask for all IOs;
    // We cannot overwrite all existing pin functions, otherwise we interrupt the MMC
    uint32_t fsel_mask[6] = {// clang-format off
        gpio->gpfsel0.value,
        gpio->gpfsel1.value,
        gpio->gpfsel2.value,
        gpio->gpfsel3.value,
        gpio->gpfsel4.value,
        gpio->gpfsel5.value
    };// clang-format on

    kraken_log_debug("Read BCM2835 GPFSEL0 register as 0x%08X", fsel_mask[0]);
    kraken_log_debug("Read BCM2835 GPFSEL1 register as 0x%08X", fsel_mask[1]);
    kraken_log_debug("Read BCM2835 GPFSEL2 register as 0x%08X", fsel_mask[2]);
    kraken_log_debug("Read BCM2835 GPFSEL3 register as 0x%08X", fsel_mask[3]);
    kraken_log_debug("Read BCM2835 GPFSEL4 register as 0x%08X", fsel_mask[4]);
    kraken_log_debug("Read BCM2835 GPFSEL5 register as 0x%08X", fsel_mask[5]);

    uint32_t clr_mask[2] = {0x00000000, 0x00000000};
    for(size_t i = 0; i < io_count; i++) {
        const kraken_io_t* io = (const kraken_io_t*) ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        // Set function selection bit for current IO
        const uint32_t fsel_bank = bcm_pin / GPIO_FSEL_BANK_SIZE;
        const uint32_t fsel_bit = bcm_pin % GPIO_FSEL_BANK_SIZE * 3;
        fsel_mask[fsel_bank] &= ~(0b111 << fsel_bit);// Clear old bits
        fsel_mask[fsel_bank] |= io->mode << fsel_bit;// 0b000 input, 0b001 output
        // Set output mask bit for current IO so we know which IOs to clear
        const uint32_t output_bank = bcm_pin / GPIO_REGISTER_SIZE;
        const uint32_t output_bit = bcm_pin % GPIO_REGISTER_SIZE;
        clr_mask[output_bank] |= io->mode << output_bit;
    }
    // Update IO functions
    kraken_log_debug("Writing 0x%08X to BCM2835 GPFSEL0 register", fsel_mask[0]);
    gpio->gpfsel0.value = fsel_mask[0];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPFSEL1 register", fsel_mask[1]);
    gpio->gpfsel1.value = fsel_mask[1];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPFSEL2 register", fsel_mask[2]);
    gpio->gpfsel2.value = fsel_mask[2];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPFSEL3 register", fsel_mask[3]);
    gpio->gpfsel3.value = fsel_mask[3];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPFSEL4 register", fsel_mask[4]);
    gpio->gpfsel4.value = fsel_mask[4];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPFSEL5 register", fsel_mask[5]);
    gpio->gpfsel5.value = fsel_mask[5];
    // Clear all outputs to their default state
    kraken_log_debug("Writing 0x%08X to BCM2835 GPCLR0 register", clr_mask[0]);
    gpio->gpclr0.value = clr_mask[0];
    kraken_log_debug("Writing 0x%08X to BCM2835 GPCLR1 register", clr_mask[1]);
    gpio->gpclr1.value = clr_mask[1];
    return KRAKEN_OK;
}