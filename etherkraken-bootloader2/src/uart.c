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

#include "uart.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>

constexpr uint32_t UART_BAUD_RATE = 115200;
constexpr uint32_t UART_BUFFER_SIZE = 128;// Same as I2C-UART bridge
volatile uint8_t g_uart_rx_buffer[UART_BUFFER_SIZE];
volatile uint8_t g_uart_tx_buffer[UART_BUFFER_SIZE];
volatile uint8_t g_uart_rx_head = 0;
volatile uint8_t g_uart_rx_tail = 0;
volatile uint8_t g_uart_tx_head = 0;
volatile uint8_t g_uart_tx_tail = 0;

static uint8_t uart_ring_next(const uint8_t current) {
    return (current + 1) % UART_BUFFER_SIZE;
}

void usart1_isr() {
    if(usart_get_flag(USART1, USART_ISR_RXNE)) {
        // TODO: ...
    }
    if(usart_get_flag(USART1, USART_ISR_TXE)) {
        if(g_uart_tx_tail != g_uart_tx_head) {
            usart_send(USART1, g_uart_tx_buffer[g_uart_tx_tail]);
            g_uart_tx_tail = uart_ring_next(g_uart_tx_tail);
        }
        else {
            usart_disable_tx_interrupt(USART1);// Buffer is empty, disable interrupt
        }
    }
}

void uart_init() {
    // Enable clocks for IO port and UART peripheral
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    // Configure UART peripheral IOs
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10 | GPIO11 | GPIO12);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO9 | GPIO10 | GPIO11 | GPIO12);
    // Configure UART peripheral
    usart_set_baudrate(USART1, UART_BAUD_RATE);               // We use maximum viable UART speed
    usart_set_databits(USART1, 8);                            // 8 data bits
    usart_set_stopbits(USART1, USART_STOPBITS_1);             // 1 stop bit
    usart_set_parity(USART1, USART_PARITY_NONE);              // No parity bit
    usart_set_flow_control(USART1, USART_FLOWCONTROL_RTS_CTS);// Hardware flow control in both directions
    usart_set_mode(USART1, USART_MODE_TX_RX);                 // We handle sending and receiving
    // Set up receive interrupt for the UART peripheral
    usart_enable_rx_interrupt(USART1);
    nvic_enable_irq(NVIC_USART1_IRQ);
    // Enable the UART interface
    usart_enable(USART1);
}

bool uart_try_write(const void* data, const uint32_t size) {
    const uint16_t next_head = uart_ring_next(g_uart_tx_head);
    if(next_head == g_uart_tx_tail) {
        return false;// Buffer is full, early return
    }
    __builtin_memcpy((void*) g_uart_tx_buffer, data, size);
    g_uart_tx_head = next_head;
    usart_enable_tx_interrupt(USART1);
    return true;
}

void uart_write(const void* data, const uint32_t size) {
    while(uart_try_write(data, size)) {
        __asm volatile("wfi");// Wait for next interrupt
    }
}