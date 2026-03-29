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
#include "kraken_io_impl.h"

constexpr uint32_t GPIO_REGISTER_SIZE = sizeof(bcm2835_gppinreg_t);
constexpr uint32_t GPIO_FSEL_BANK_SIZE = 10;

KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_update(void* base_address, void* shadow_memory,
                                                       const kraken_io_c_handle_t* ios, const size_t io_count) {
    volatile bcm2835_gpio_t* gpio = base_address;
    // Capture input state at the start of the update
    const uint32_t input_state_mask[] = {gpio->gplev0.value, gpio->gplev1.value};

    bcm2835_gpio_t* shadow_gpio = shadow_memory;
    // Compose the output state masks for the current io state
    uint32_t output_state_mask[2];
    for(size_t io_index = 0; io_index < io_count; io_index++) {
        kraken_io_t* io = (kraken_io_t*) ios[io_index];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const uint32_t bank = bcm_pin / GPIO_REGISTER_SIZE;
        const uint32_t bit = bcm_pin % GPIO_REGISTER_SIZE;
        // When mode is INPUT (0), the inverse (1) will enable state check for the current IO
        io->state = (kraken_bool_t) (input_state_mask[bank] & ~io->mode << bit);
        // IO state bit AND IO mode bit (1/OUT) will override the output bit for the current IO
        output_state_mask[bank] |= ((uint8_t) io->state & io->mode & 0b1) << bit;
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
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_init(void* base_address, void* shadow_memory,
                                                     const kraken_io_c_handle_t* ios, const size_t io_count) {
    volatile bcm2835_gpio_t* gpio = base_address;
    memset(shadow_memory, 0x00, sizeof(bcm2835_gpio_t));
    // Build function selection mask for all IOs
    uint32_t fsel_mask[2];
    uint32_t output_mask[2];
    for(size_t i = 0; i < io_count; i++) {
        const kraken_io_t* io = (const kraken_io_t*) ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        // Set function selection bit for current IO
        const uint32_t fsel_bank = bcm_pin / GPIO_FSEL_BANK_SIZE;
        const uint32_t fsel_bit = bcm_pin % GPIO_FSEL_BANK_SIZE * 3;
        fsel_mask[fsel_bank] |= (io->mode & 0b111) << fsel_bit;
        // Set output mask bit for current IO so we know which IOs to clear
        const uint32_t output_bank = bcm_pin / GPIO_REGISTER_SIZE;
        const uint32_t output_bit = bcm_pin % GPIO_REGISTER_SIZE;
        output_mask[output_bank] |= (io->mode & 0b1) << output_bit;
    }
    // Update IO functions
    gpio->gpfsel0.value = fsel_mask[0];
    gpio->gpfsel1.value = fsel_mask[1];
    // Clear all outputs to their default state
    gpio->gpclr0.value = output_mask[0];
    gpio->gpclr1.value = output_mask[1];
    return KRAKEN_OK;
}