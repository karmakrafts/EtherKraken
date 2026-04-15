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

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>

#include "bl2_uart.h"

void dma1_channel2_3_dma2_channel1_2_isr() {
    if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL2, DMA_TCIF)) {
        dma_clear_interrupt_flags(DMA1, DMA_CHANNEL2, DMA_TCIF);
        bl2_uart_handle_tx_irq();// Handle UART transmit completion IRQ
    }
}