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

/**
 * bootloader2 is started to stream firmware updates into memory
 * and flash them persistently to the coprocessors internal flash.
 * It performs the following actions when started:
 *
 * - Configure UART interface (with HFC)
 * - Send acknowledgement to main processor via UART
 * - Wait for main processor to respond with boot command
 *  -> 0x00: boot from internal flash (soft reset system)
 *      soft reset coprocessor
 *  -> 0x01: receive firmware and store in RAM
 *      receive length prefixed binary blob via UART
 *  -> 0x02: boot from RAM
 *      jump to the start address of the received binary blob
 *  -> 0x03: flash firmware stored in RAM to internal flash
 *      copy the received binary blob to the internal flash
 */

#include "bl2_protocol.h"
#include "uart.h"

static void send_ack() {
    const bl2_c2m_command_packet_t packet = {// clang-format off
        .type = BL2_C2M_PACKET_TYPE_COMMAND,
        .command = BL2_C2M_COMMAND_ACK
    };// clang-format on
    uart_write(&packet, sizeof(packet));
}

int main() {
    uart_init();// We take over the UART from the ROM bootloader
    send_ack(); // Let main processor know we started up
    return 0;
}