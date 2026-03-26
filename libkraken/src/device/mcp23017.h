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

#include "kraken_api.h"

#define MCP23017_DEFINE_FLAG_STATE(s, S, Z, O)                                                                         \
    typedef enum mcp23017_##s##_state : uint8_t {MCP23017_##S##_STATE_##Z,                                             \
                                                 MCP23017_##S##_STATE_##O} mcp23017_##s##_state_t;

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

#define MCP23017_DEFINE_REGISTER_UNION(s)                                                                              \
    typedef struct mcp23017_##s {                                                                                      \
        union {                                                                                                        \
            mcp23017_##s##_flags_t flags;                                                                              \
            uint8_t value;                                                                                             \
        };                                                                                                             \
    } mcp23017_##s##_t;

#define MCP23017_DEFINE_REGISTER(s, S, Z, O)                                                                           \
    MCP23017_DEFINE_FLAG_STATE(s, S, Z, O)                                                                             \
    MCP23017_DEFINE_FLAGS(s)                                                                                           \
    MCP23017_DEFINE_REGISTER_UNION(s)

typedef enum mcp23017_register : uint8_t {
    MCP23017_REGISTER_IODIRA = 0x00,
    MCP23017_REGISTER_IODIRB = 0x01,
    MCP23017_REGISTER_IPOLA = 0x02,
    MCP23017_REGISTER_IPOLB = 0x03,
    MCP23017_REGISTER_GPINTENA = 0x04,
    MCP23017_REGISTER_GPINTENB = 0x05,
    MCP23017_REGISTER_DEFVALA = 0x06,
    MCP23017_REGISTER_DEFVALB = 0x07,
    MCP23017_REGISTER_INTCONA = 0x08,
    MCP23017_REGISTER_INTCONB = 0x09,
    MCP23017_REGISTER_IOCON = 0x0A,// 0x0B maps to the same register
    MCP23017_REGISTER_GPPUA = 0x0C,
    MCP23017_REGISTER_GPPUB = 0x0D,
    MCP23017_REGISTER_INTFA = 0x0E,
    MCP23017_REGISTER_INTFB = 0x0F,
    MCP23017_REGISTER_INTCAPA = 0x10,
    MCP23017_REGISTER_INTCAPB = 0x11,
    MCP23017_REGISTER_GPIOA = 0x12,
    MCP23017_REGISTER_GPIOB = 0x13,
    MCP23017_REGISTER_OLATA = 0x14,
    MCP23017_REGISTER_OLATB = 0x15
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

typedef enum mcp23017_iocon_bank : uint8_t {
    MCP23017_IOCON_BANK_0,// Sequential addressing
    MCP23017_IOCON_BANK_1 // Separate banks (split ports)
} mcp23017_iocon_bank_t;

typedef enum mcp23017_intpol_state : uint8_t {
    MCP23017_INTPOL_STATE_ACTIVE_LOW,
    MCP23017_INTPOL_STATE_ACTIVE_HIGH
} mcp23017_intpol_state_t;

typedef enum mcp23017_odr_state : uint8_t {
    MCP23017_ODR_STATE_ACTIVE_DRIVER,
    MCP23017_ODR_STATE_OPEN_DRAIN
} mcp23017_odr_state_t;

typedef enum mcp23017_disslw_state : uint8_t {
    MCP23017_DISSLW_STATE_ENABLED,
    MCP23017_DISSLW_STATE_DISABLED
} mcp23017_disslw_state_t;

typedef enum mcp23017_seqop_state : uint8_t {
    MCP23017_SEQOP_STATE_ENABLED,
    MCP23017_SEQOP_STATE_DISABLED
} mcp23017_seqop_state_t;

typedef enum mcp23017_mirror_state : uint8_t {
    MCP23017_MIRROR_STATE_SEPARATE,
    MCP23017_MIRROR_STATE_MIRRORED
} mcp23017_mirror_state_t;

typedef struct mcp23017_iocon_flags {
    uint8_t _reserved0 : 1;
    mcp23017_intpol_state_t intpol : 1;
    mcp23017_odr_state_t odr : 1;
    uint8_t _reserved1 : 1;
    mcp23017_disslw_state_t disslw : 1;
    mcp23017_seqop_state_t seqop : 1;
    mcp23017_mirror_state_t mirror : 1;
    mcp23017_iocon_bank_t bank : 1;
} mcp23017_iocon_flags_t;

#endif//LIBKRAKEN_MCP23017_H