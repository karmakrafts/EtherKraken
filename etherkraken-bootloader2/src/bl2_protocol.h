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

#ifndef BOOTLOADER2_BL2_PROTOCOL_H
#define BOOTLOADER2_BL2_PROTOCOL_H

#include <stdint.h>

constexpr uint32_t BL2_CHUNK_SIZE = 32;

// Main-to-coprocessor packets (m2c)

typedef enum bl2_m2c_packet_type_t : uint8_t {
    BL2_M2C_PACKET_ACK,
    BL2_M2C_PACKET_BOOT,
    BL2_M2C_PACKET_FLASH
} bl2_m2c_packet_type_t;

typedef struct bl2_m2c_ack_packet {
    bl2_m2c_packet_type_t type;
} bl2_m2c_ack_packet_t;

typedef struct bl2_m2c_boot_packet {
    bl2_m2c_packet_type_t type;
} bl2_m2c_boot_packet_t;

typedef struct bl2_m2c_flash_packet {
    bl2_m2c_packet_type_t type;
    uint8_t data[BL2_CHUNK_SIZE];
} bl2_m2c_flash_packet_t;

typedef struct bl2_m2c_packet {
    union {
        bl2_m2c_packet_type_t type;
        bl2_m2c_ack_packet_t ack;
        bl2_m2c_boot_packet_t boot;
        bl2_m2c_flash_packet_t flash;
    };
} bl2_m2c_packet_t;

static_assert(sizeof(bl2_m2c_packet_t) <= 128, "bl2_m2c_packet_t must be <= 128 bytes");

// Co-to-main-processor packets (c2m)

typedef enum bl2_c2m_packet_type : uint8_t {
    BL2_C2M_PACKET_TYPE_ACK,
    BL2_C2M_PACKET_TYPE_CRASH
} bl2_c2m_packet_type_t;

typedef struct bl2_c2m_ack_packet {
    bl2_c2m_packet_type_t type;
} bl2_c2m_ack_packet_t;

typedef struct bl2_c2m_crash_packet {
    bl2_c2m_packet_type_t type;
    // TODO: fill in crash information
} bl2_c2m_crash_packet_t;

typedef struct bl2_c2m_packet {
    union {
        bl2_c2m_packet_type_t type;
        bl2_c2m_ack_packet_t ack;
        bl2_c2m_crash_packet_t crash;
    };
} bl2_c2m_packet_t;

static_assert(sizeof(bl2_c2m_packet_t) <= 128, "bl2_c2m_packet_t must be <= 128 bytes");

#endif//BOOTLOADER2_BL2_PROTOCOL_H