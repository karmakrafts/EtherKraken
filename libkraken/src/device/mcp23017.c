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

#include "../../include/device/mcp23017.h"
#include "kraken_io_impl.h"
#include "util/kraken_internal.h"

#include <unistd.h>

constexpr size_t MCP23017_REGISTER_SIZE = 8;

KRAKEN_EXPORT kraken_error_t mcp23017_i2c_mux_state_update(const int fd, void*, kraken_io_handle_t* ios,
                                                           const size_t io_count, const uint64_t mask) {
    constexpr uint8_t input_register[] = {MCP23017_REGISTER_GPIOA};
    KRAKEN_CHECK_CALL(write(fd, input_register, sizeof(input_register)) > 0, KRAKEN_ERR_INVALID_ARG,
                      "Could not update MUX register pointer");// Set register pointer to GPIOA
    uint8_t input_data[] = {0x00, 0x00};
    KRAKEN_CHECK_CALL(read(fd, input_data, sizeof(input_data)) > 0, KRAKEN_ERR_INVALID_ARG,
                      "Could not read MUX GPIO registers");// Read GPIOA and GPIOB in one go

    uint8_t output_data[] = {MCP23017_REGISTER_GPIOA, 0x00, 0x00};
    for(size_t i = 0; i < io_count; i++) {
        kraken_io_t* io = ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const size_t bank = bcm_pin / MCP23017_REGISTER_SIZE;
        const size_t bit = bcm_pin % MCP23017_REGISTER_SIZE;
        if(io->mode == KRAKEN_IO_MODE_IN) {
            io->state = (kraken_bool_t) ((input_data[bank] & 0b1) << bit);
            continue;
        }
        output_data[bank + 1] |= (io->mode & 0b1) << bit;
    }
    KRAKEN_CHECK_CALL(write(fd, output_data, sizeof(output_data)) > 0, KRAKEN_ERR_INVALID_ARG,
                      "Could not update MUX GPIO registers");// Update output state

    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t mcp23017_i2c_mux_state_init(const int fd, void*, kraken_io_handle_t* ios,
                                                         const size_t io_count, const uint64_t mask) {
    // Update IOCON register to ensure banking is disabled and SEQOP bit is set
    // clang-format off
    const mcp23017_iocon_t iocon = {
        .flags.bank = MCP23017_IOCON_BANK_0,
        .flags.seqop = MCP23017_SEQOP_STATE_ENABLED// Ensure register auto-increment works
    };
    // clang-format on
    const uint8_t iocon_data[] = {MCP23017_REGISTER_IOCON, iocon.value};
    KRAKEN_CHECK_CALL(write(fd, iocon_data, sizeof(iocon_data)) > 0, KRAKEN_ERR_INVALID_OP,
                      "Could not update MUX IOCON register");
    // Update pin directions (using sequential write to reduce syscalls)
    uint8_t direction_data[] = {MCP23017_REGISTER_IODIRA, 0x00, 0x00};
    for(size_t i = 0; i < io_count; i++) {
        const kraken_io_t* io = ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const size_t bank = bcm_pin / MCP23017_REGISTER_SIZE;
        const size_t bit = bcm_pin % MCP23017_REGISTER_SIZE;
        direction_data[bank + 1] |= (~io->mode & 0b1) << bit;
    }
    KRAKEN_CHECK_CALL(write(fd, direction_data, sizeof(direction_data)) > 0, KRAKEN_ERR_INVALID_OP,
                      "Could not update MUX IODIR registers");
    // Clear pin state of all outputs
    constexpr uint8_t gpio_data[] = {MCP23017_REGISTER_GPIOA, 0x00, 0x00};
    KRAKEN_CHECK_CALL(write(fd, gpio_data, sizeof(gpio_data)) > 0, KRAKEN_ERR_INVALID_OP,
                      "Could not update MUX GPIO registers");
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t mcp23017_spi_mux_state_update(void* base_address, void*, kraken_io_handle_t* ios,
                                                           const size_t io_count, const uint64_t mask) {
    volatile mcp23017_t* registers = base_address;
    const uint8_t input_state[] = {registers->gpioa.value, registers->gpiob.value};

    uint8_t output_state[] = {0x00, 0x00};
    for(size_t i = 0; i < io_count; i++) {
        kraken_io_t* io = ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const size_t bank = bcm_pin / MCP23017_REGISTER_SIZE;
        const size_t bit = bcm_pin % MCP23017_REGISTER_SIZE;
        if(io->mode == KRAKEN_IO_MODE_IN) {
            io->state = (kraken_bool_t) ((input_state[bank] & 0b1) << bit);
            continue;
        }
        output_state[bank] |= (io->state & 0b1) << bit;
    }
    registers->gpioa.value = output_state[0];
    registers->gpiob.value = output_state[1];

    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t mcp23017_spi_mux_state_init(void* base_address, void*, kraken_io_handle_t* ios,
                                                         const size_t io_count, const uint64_t mask) {
    volatile mcp23017_t* registers = base_address;
    // Update IOCON register to ensure banking is disabled and SEQOP bit is cleared
    // clang-format off
    const mcp23017_iocon_t iocon = {
        .flags.bank = MCP23017_IOCON_BANK_0,
        .flags.seqop = MCP23017_SEQOP_STATE_DISABLED
    };
    // clang-format on
    registers->iocona.value = iocon.value;

    // Set pin directions
    uint8_t direction_data[] = {0x00, 0x00};
    for(size_t i = 0; i < io_count; i++) {
        const kraken_io_t* io = ios[i];
        const uint32_t bcm_pin = io->pin_config.device_pin;
        const size_t bank = bcm_pin / MCP23017_REGISTER_SIZE;
        const size_t bit = bcm_pin % MCP23017_REGISTER_SIZE;
        direction_data[bank] |= (~io->mode & 0b1) << bit;
    }
    registers->iodira.value = direction_data[0];
    registers->iodirb.value = direction_data[1];

    // Reset output state
    registers->gpioa.value = 0x00;
    registers->gpiob.value = 0x00;

    return KRAKEN_OK;
}