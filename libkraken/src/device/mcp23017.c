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
#include "kraken_internal.h"
#include "kraken_io_impl.h"

#include <unistd.h>

constexpr size_t MCP23017_REGISTER_SIZE = 8;

KRAKEN_EXPORT kraken_error_t mcp23017_i2c_mux_state_update(int fd, void* shadow_memory, const kraken_io_c_handle_t* ios,
                                                           size_t io_count) {
    mcp23017_t* shadow_registers = shadow_memory;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t mcp23017_i2c_mux_state_init(const int fd, void*, const kraken_io_c_handle_t* ios,
                                                         const size_t io_count) {
    // Update IOCON register to ensure banking is disabled and SEQOP bit is set
    // clang-format off
    const mcp23017_iocon_t iocon = {
        .flags.bank = MCP23017_IOCON_BANK_0,
        .flags.seqop = MCP23017_SEQOP_STATE_ENABLED// Ensure register auto-increment works
    };
    // clang-format on
    const uint8_t iocon_data[] = {MCP23017_REGISTER_IOCON, iocon.value};
    KRAKEN_CHECK(write(fd, iocon_data, sizeof(iocon_data)) > 0, KRAKEN_ERR_INVALID_OP,
                 "Could not update MUX IOCON register");
    // Update pin directions (using sequential write to reduce syscalls)
    uint8_t direction_data[] = {MCP23017_REGISTER_IODIRA, 0x00, 0x00};
    for(size_t i = 0; i < io_count; i++) {
        const kraken_io_t* io = (const kraken_io_t*) ios[i];
        const size_t bank = i / MCP23017_REGISTER_SIZE;
        const size_t bit = i % MCP23017_REGISTER_SIZE;
        direction_data[bank + 1] |= ~io->mode << bit;
    }
    KRAKEN_CHECK(write(fd, direction_data, sizeof(direction_data)) > 0, KRAKEN_ERR_INVALID_OP,
                 "Could not update MUX IODIR registers");
    // Clear pin state of all outputs
    constexpr uint8_t gpio_data[] = {MCP23017_REGISTER_GPIOA, 0x00, 0x00};
    KRAKEN_CHECK(write(fd, gpio_data, sizeof(gpio_data)) > 0, KRAKEN_ERR_INVALID_OP,
                 "Could not update MUX GPIO registers");
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t mcp23017_spi_mux_state_update(void* base_address, void* shadow_memory,
                                                           const kraken_io_c_handle_t* ios, size_t io_count) {
    volatile mcp23017_t* registers = base_address;
    mcp23017_t* shadow_registers = shadow_memory;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t mcp23017_spi_mux_state_init(void* base_address, void*, const kraken_io_c_handle_t* ios,
                                                         size_t io_count) {
    volatile mcp23017_t* registers = base_address;
    // Update IOCON register to ensure banking is disabled and SEQOP bit is cleared
    // clang-format off
    const mcp23017_iocon_t iocon = {
        .flags.bank = MCP23017_IOCON_BANK_0,
        .flags.seqop = MCP23017_SEQOP_STATE_DISABLED
    };
    // clang-format on
    registers->iocona.value = iocon.value;
    return KRAKEN_OK;
}