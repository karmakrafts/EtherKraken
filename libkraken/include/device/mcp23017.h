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

#ifndef LIBKRAKEN_MCP23017_H
#define LIBKRAKEN_MCP23017_H

/**
 * @see https://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf
 */

#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

/// @brief Defines a flag state enum for a specific register flag
/// @param s The snake_case name of the flag
/// @param S The SCREAMING_SNAKE_CASE name of the flag
/// @param Z The name of the zero state (0)
/// @param O The name of the one state (1)
#define MCP23017_DEFINE_FLAG_STATE(s, S, Z, O)                                                                         \
    typedef enum mcp23017_##s##_state : uint8_t {MCP23017_##S##_STATE_##Z,                                             \
                                                 MCP23017_##S##_STATE_##O} mcp23017_##s##_state_t;

/// @brief Defines a bitfield struct for a register
/// @param s The snake_case name of the register
#define MCP23017_DEFINE_FLAGS(s)                                                                                       \
    typedef struct mcp23017_##s##_flags {                                                                              \
        mcp23017_##s##_state_t gp0 : 1;                                                                                \
        mcp23017_##s##_state_t gp1 : 1;                                                                                \
        mcp23017_##s##_state_t gp2 : 1;                                                                                \
        mcp23017_##s##_state_t gp3 : 1;                                                                                \
        mcp23017_##s##_state_t gp4 : 1;                                                                                \
        mcp23017_##s##_state_t gp5 : 1;                                                                                \
        mcp23017_##s##_state_t gp6 : 1;                                                                                \
        mcp23017_##s##_state_t gp7 : 1;                                                                                \
    } mcp23017_##s##_flags_t;

/// @brief Defines a union for a register to allow both bitfield and raw value access
/// @param s The snake_case name of the register
#define MCP23017_DEFINE_REGISTER_UNION(s)                                                                              \
    typedef struct mcp23017_##s {                                                                                      \
        union {                                                                                                        \
            mcp23017_##s##_flags_t flags;                                                                              \
            uint8_t value;                                                                                             \
        };                                                                                                             \
    } mcp23017_##s##_t;

/// @brief Defines all necessary types for a register
/// @param s The snake_case name of the register
/// @param S The SCREAMING_SNAKE_CASE name of the register
/// @param Z The name of the zero state (0)
/// @param O The name of the one state (1)
#define MCP23017_DEFINE_REGISTER(s, S, Z, O)                                                                           \
    MCP23017_DEFINE_FLAG_STATE(s, S, Z, O)                                                                             \
    MCP23017_DEFINE_FLAGS(s)                                                                                           \
    MCP23017_DEFINE_REGISTER_UNION(s)

/// @brief MCP23017 register addresses
typedef enum mcp23017_register : uint8_t {
    MCP23017_REGISTER_IODIRA = 0x00,  ///< I/O Direction Register A
    MCP23017_REGISTER_IODIRB = 0x01,  ///< I/O Direction Register B
    MCP23017_REGISTER_IPOLA = 0x02,   ///< Input Polarity Port Register A
    MCP23017_REGISTER_IPOLB = 0x03,   ///< Input Polarity Port Register B
    MCP23017_REGISTER_GPINTENA = 0x04,///< Interrupt-on-Change Control Register A
    MCP23017_REGISTER_GPINTENB = 0x05,///< Interrupt-on-Change Control Register B
    MCP23017_REGISTER_DEFVALA = 0x06, ///< Default Value Register A
    MCP23017_REGISTER_DEFVALB = 0x07, ///< Default Value Register B
    MCP23017_REGISTER_INTCONA = 0x08, ///< Interrupt-on-Change Control Register A
    MCP23017_REGISTER_INTCONB = 0x09, ///< Interrupt-on-Change Control Register B
    MCP23017_REGISTER_IOCON = 0x0A,   ///< I/O Expander Configuration Register (0x0B maps to the same register)
    MCP23017_REGISTER_GPPUA = 0x0C,   ///< GPIO Pull-Up Resistor Register A
    MCP23017_REGISTER_GPPUB = 0x0D,   ///< GPIO Pull-Up Resistor Register B
    MCP23017_REGISTER_INTFA = 0x0E,   ///< Interrupt Flag Register A
    MCP23017_REGISTER_INTFB = 0x0F,   ///< Interrupt Flag Register B
    MCP23017_REGISTER_INTCAPA = 0x10, ///< Interrupt Capture Register A
    MCP23017_REGISTER_INTCAPB = 0x11, ///< Interrupt Capture Register B
    MCP23017_REGISTER_GPIOA = 0x12,   ///< Port Register A
    MCP23017_REGISTER_GPIOB = 0x13,   ///< Port Register B
    MCP23017_REGISTER_OLATA = 0x14,   ///< Output Latch Register A
    MCP23017_REGISTER_OLATB = 0x15    ///< Output Latch Register B
} mcp23017_register_t;

MCP23017_DEFINE_REGISTER(iodir, IODIR, OUTPUT, INPUT)
MCP23017_DEFINE_REGISTER(ipol, IPOL, NORMAL, INVERTED)
MCP23017_DEFINE_REGISTER(gpinten, GPINTEN, DISABLED, ENABLED)
MCP23017_DEFINE_REGISTER(defval, DEFVAL, LOW, HIGH)
MCP23017_DEFINE_REGISTER(intcon, INTCON, PREVIOUS, DEFVAL)
MCP23017_DEFINE_REGISTER(gppu, GPPU, DISABLED, ENABLED)
MCP23017_DEFINE_REGISTER(intf, INTF, RESET, TRIGGERED)
MCP23017_DEFINE_REGISTER(intcap, INTCAP, LOW, HIGH)
MCP23017_DEFINE_REGISTER(gpio, GPIO, LOW, HIGH)
MCP23017_DEFINE_REGISTER(olat, OLAT, LOW, HIGH)

/// @brief IOCON bank mode
typedef enum mcp23017_iocon_bank : uint8_t {
    MCP23017_IOCON_BANK_0,///< Sequential addressing
    MCP23017_IOCON_BANK_1 ///< Separate banks (split ports)
} mcp23017_iocon_bank_t;

/// @brief Interrupt polarity state
typedef enum mcp23017_intpol_state : uint8_t {
    MCP23017_INTPOL_STATE_ACTIVE_LOW,///< Interrupt pin active low
    MCP23017_INTPOL_STATE_ACTIVE_HIGH///< Interrupt pin active high
} mcp23017_intpol_state_t;

/// @brief Open-drain output control state
typedef enum mcp23017_odr_state : uint8_t {
    MCP23017_ODR_STATE_ACTIVE_DRIVER,///< Active driver output
    MCP23017_ODR_STATE_OPEN_DRAIN    ///< Open-drain output
} mcp23017_odr_state_t;

/// @brief Slew rate control state
typedef enum mcp23017_disslw_state : uint8_t {
    MCP23017_DISSLW_STATE_ENABLED,///< Slew rate control enabled
    MCP23017_DISSLW_STATE_DISABLED///< Slew rate control disabled
} mcp23017_disslw_state_t;

/// @brief Sequential operation control state
typedef enum mcp23017_seqop_state : uint8_t {
    MCP23017_SEQOP_STATE_ENABLED,///< Sequential operation enabled (address pointer increments)
    MCP23017_SEQOP_STATE_DISABLED///< Sequential operation disabled (address pointer does not increment)
} mcp23017_seqop_state_t;

/// @brief Interrupt mirror control state
typedef enum mcp23017_mirror_state : uint8_t {
    MCP23017_MIRROR_STATE_SEPARATE,///< INTA and INTB are separate
    MCP23017_MIRROR_STATE_MIRRORED ///< INTA and INTB are internally connected
} mcp23017_mirror_state_t;

/// @brief IOCON register flags
typedef struct mcp23017_iocon_flags {
    uint8_t _reserved0 : 1;            ///< Reserved
    mcp23017_intpol_state_t intpol : 1;///< Interrupt polarity
    mcp23017_odr_state_t odr : 1;      ///< Open-drain control
    uint8_t _reserved1 : 1;            ///< Reserved
    mcp23017_disslw_state_t disslw : 1;///< Slew rate control
    mcp23017_seqop_state_t seqop : 1;  ///< Sequential operation control
    mcp23017_mirror_state_t mirror : 1;///< Interrupt mirror control
    mcp23017_iocon_bank_t bank : 1;    ///< Bank mode
} mcp23017_iocon_flags_t;

/// @brief IOCON register
typedef struct mcp23017_iocon {
    union {
        mcp23017_iocon_flags_t flags;///< Bitfield access
        uint8_t value;               ///< Raw value access
    };
} mcp23017_iocon_t;

/// @brief MCP23017 full register map state
typedef struct mcp23017 {
    mcp23017_iodir_t iodira;    ///< I/O direction register A
    mcp23017_iodir_t iodirb;    ///< I/O direction register B
    mcp23017_ipol_t ipola;      ///< Input polarity register A
    mcp23017_ipol_t ipolb;      ///< Input polarity register B
    mcp23017_gpinten_t gpintena;///< Interrupt-on-change control register A
    mcp23017_gpinten_t gpintenb;///< Interrupt-on-change control register B
    mcp23017_defval_t defvala;  ///< Default value register A
    mcp23017_defval_t defvalb;  ///< Default value register B
    mcp23017_intcon_t intcona;  ///< Interrupt-on-change control register A
    mcp23017_intcon_t intconb;  ///< Interrupt-on-change control register B
    mcp23017_iocon_t iocona;    ///< I/O configuration register A
    mcp23017_iocon_t ioconb;    ///< I/O configuration register B (same as iocona)
    mcp23017_gppu_t gppua;      ///< GPIO pull-up resistor register A
    mcp23017_gppu_t gppub;      ///< GPIO pull-up resistor register B
    mcp23017_intf_t intfa;      ///< Interrupt flag register A
    mcp23017_intf_t intfb;      ///< Interrupt flag register B
    mcp23017_intcap_t intcapa;  ///< Interrupt capture register A
    mcp23017_intcap_t intcapb;  ///< Interrupt capture register B
    mcp23017_gpio_t gpioa;      ///< Port register A
    mcp23017_gpio_t gpiob;      ///< Port register B
    mcp23017_olat_t olata;      ///< Output latch register A
    mcp23017_olat_t olatb;      ///< Output latch register B
} mcp23017_t;

/// @brief Updates the MCP23017 state via I2C
/// @param fd The I2C file descriptor
/// @param shadow_memory The shadow memory for the MCP23017 state (mcp23017_t)
/// @param ios The I/O handles to update
/// @param io_count The number of I/O handles
/// @return KRAKEN_ERROR_OK on success, or an error code
KRAKEN_EXPORT kraken_error_t mcp23017_i2c_mux_state_update(int fd, void* shadow_memory, kraken_io_handle_t* ios,
                                                           size_t io_count);

/// @brief Initializes the MCP23017 state via I2C
/// @param fd The I2C file descriptor
/// @param shadow_memory The shadow memory for the MCP23017 state (mcp23017_t)
/// @param ios The I/O handles to initialize
/// @param io_count The number of I/O handles
/// @return KRAKEN_ERROR_OK on success, or an error code
KRAKEN_EXPORT kraken_error_t mcp23017_i2c_mux_state_init(int fd, void* shadow_memory, kraken_io_handle_t* ios,
                                                         size_t io_count);

/// @brief Updates the MCP23017 state via SPI
/// @param base_address The SPI base address
/// @param shadow_memory The shadow memory for the MCP23017 state (mcp23017_t)
/// @param ios The I/O handles to update
/// @param io_count The number of I/O handles
/// @return KRAKEN_ERROR_OK on success, or an error code
KRAKEN_EXPORT kraken_error_t mcp23017_spi_mux_state_update(void* base_address, void* shadow_memory,
                                                           kraken_io_handle_t* ios, size_t io_count);

/// @brief Initializes the MCP23017 state via SPI
/// @param base_address The SPI base address
/// @param shadow_memory The shadow memory for the MCP23017 state (mcp23017_t)
/// @param ios The I/O handles to initialize
/// @param io_count The number of I/O handles
/// @return KRAKEN_ERROR_OK on success, or an error code
KRAKEN_EXPORT kraken_error_t mcp23017_spi_mux_state_init(void* base_address, void* shadow_memory,
                                                         kraken_io_handle_t* ios, size_t io_count);

KRAKEN_API_END

#endif//LIBKRAKEN_MCP23017_H