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

#ifndef BOOTLOADER2_UART_H
#define BOOTLOADER2_UART_H

#include <stdint.h>

void bl2_uart_handle_tx_irq();
void bl2_uart_init();
void bl2_uart_write(const void* data, uint16_t size);
uint16_t bl2_uart_read(void* data, uint16_t size);

#endif//BOOTLOADER2_UART_H