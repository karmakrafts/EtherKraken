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

#include "bl2_uart.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

constexpr uint32_t UART_BAUD_RATE = 115200;
constexpr uint16_t UART_BUFFER_SIZE = 128;// Same as I2C-UART bridge
constexpr uint16_t UART_TX_PIN = GPIO9;
constexpr uint16_t UART_RX_PIN = GPIO10;
constexpr uint16_t UART_CTS_PIN = GPIO11;
constexpr uint16_t UART_RTS_PIN = GPIO12;
constexpr uint16_t UART_PINS = UART_TX_PIN | UART_RX_PIN | UART_CTS_PIN | UART_RTS_PIN;

static volatile uint8_t g_rx_buffer[UART_BUFFER_SIZE] = {};
static uint16_t g_rx_tail = 0;

static volatile uint8_t g_tx_buffer[UART_BUFFER_SIZE] = {};
static volatile uint16_t g_tx_head = 0;
static volatile uint16_t g_tx_tail = 0;
static volatile bool g_tx_dma_active = false;

static void configure_clocks() {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_DMA1);
}

static void configure_gpio() {
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_PINS);
    gpio_set_af(GPIOA, GPIO_AF1, UART_PINS);
}

static void configure_uart() {
    usart_set_baudrate(USART1, UART_BAUD_RATE);               // We use maximum viable UART speed
    usart_set_databits(USART1, 8);                            // 8 data bits
    usart_set_stopbits(USART1, USART_STOPBITS_1);             // 1 stop bit
    usart_set_parity(USART1, USART_PARITY_NONE);              // No parity bit
    usart_set_flow_control(USART1, USART_FLOWCONTROL_RTS_CTS);// Hardware flow control in both directions
    usart_set_mode(USART1, USART_MODE_TX_RX);                 // We handle sending and receiving
    usart_enable(USART1);                                     // Enable the UART interface
    usart_enable_tx_dma(USART1);                              // Enable DMA for transmitting
    usart_enable_rx_dma(USART1);                              // Enable DMA for receiving
}

static void configure_tx_dma() {
    dma_channel_reset(DMA1, DMA_CHANNEL2);// USART1_TX
    dma_set_peripheral_address(DMA1, DMA_CHANNEL2, (uint32_t) &USART_TDR(USART1));
    dma_set_peripheral_size(DMA1, DMA_CHANNEL2, DMA_CCR_PSIZE_8BIT);// Unit size of UART word
    dma_set_memory_address(DMA1, DMA_CHANNEL2, (uint32_t) g_tx_buffer);
    dma_set_memory_size(DMA1, DMA_CHANNEL2, DMA_CCR_MSIZE_8BIT);// Unit size of memory word
    dma_set_number_of_data(DMA1, DMA_CHANNEL2, UART_BUFFER_SIZE);
    dma_set_read_from_memory(DMA1, DMA_CHANNEL2);
    dma_set_priority(DMA1, DMA_CHANNEL2, DMA_CCR_PL_LOW);
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL2);
    dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL2);
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL2);
    nvic_enable_irq(NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ);// Ensure we can handle DMA transfer complete IRQs
}

static void configure_rx_dma() {
    dma_channel_reset(DMA1, DMA_CHANNEL3);// USART1_RX
    dma_set_peripheral_address(DMA1, DMA_CHANNEL3, (uint32_t) &USART_RDR(USART1));
    dma_set_peripheral_size(DMA1, DMA_CHANNEL3, DMA_CCR_PSIZE_8BIT);// Unit size of UART word
    dma_set_memory_address(DMA1, DMA_CHANNEL3, (uint32_t) g_rx_buffer);
    dma_set_memory_size(DMA1, DMA_CHANNEL3, DMA_CCR_MSIZE_8BIT);// Unit size of memory word
    dma_set_number_of_data(DMA1, DMA_CHANNEL3, UART_BUFFER_SIZE);
    dma_set_read_from_peripheral(DMA1, DMA_CHANNEL3);
    dma_set_priority(DMA1, DMA_CHANNEL3, DMA_CCR_PL_LOW);
    dma_enable_circular_mode(DMA1, DMA_CHANNEL3);// Use a hardware ring-buffer for received data
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL3);
}

static void start_tx_dma_if_needed() {
    if(g_tx_dma_active || g_tx_head == g_tx_tail) {
        return;
    }
    uint16_t size;
    if(g_tx_head > g_tx_tail) {
        size = g_tx_head - g_tx_tail;
    }
    else {
        size = UART_BUFFER_SIZE - g_tx_tail;
    }
    dma_disable_channel(DMA1, DMA_CHANNEL2);
    dma_set_memory_address(DMA1, DMA_CHANNEL2, (uint32_t) &g_tx_buffer[g_tx_tail]);
    dma_set_number_of_data(DMA1, DMA_CHANNEL2, size);
    g_tx_dma_active = true;
    dma_enable_channel(DMA1, DMA_CHANNEL2);
}

void bl2_uart_handle_tx_irq() {
    dma_disable_channel(DMA1, DMA_CHANNEL2);
    uint16_t size;
    if(g_tx_head > g_tx_tail) {
        size = g_tx_head - g_tx_tail;
    }
    else {
        size = UART_BUFFER_SIZE - g_tx_tail;
    }
    g_tx_tail = (g_tx_tail + size) % UART_BUFFER_SIZE;
    g_tx_dma_active = false;
    start_tx_dma_if_needed();
}

void bl2_uart_init() {
    configure_clocks();
    configure_gpio();
    configure_uart();
    configure_tx_dma();
    configure_rx_dma();
}

void bl2_uart_write(const void* data, const uint16_t size) {
    // TODO: implement this
}

uint16_t bl2_uart_read(void* data, const uint16_t size) {
    if(!data || size == 0) {
        return 0;
    }

    const uint16_t head = UART_BUFFER_SIZE - dma_get_number_of_data(DMA1, DMA_CHANNEL3);
    if(g_rx_tail == head) {
        return 0;// No new data has arrived
    }
    uint16_t received_size = head - g_rx_tail;
    if(received_size > size) {
        received_size = size;// Clamp to maximum size
    }
    if(head > g_rx_tail) {// Handle a regular linear copy of the memory
        __builtin_memcpy(data, (void*) &g_rx_buffer[g_rx_tail], received_size);
    }
    else {// Handle wrapped around data
        const uint16_t slice_size = UART_BUFFER_SIZE - g_rx_tail;
        __builtin_memcpy(data, (void*) &g_rx_buffer[g_rx_tail], slice_size);
        __builtin_memcpy(&((uint8_t*) data)[slice_size], (void*) g_rx_buffer, head);
    }
    g_rx_tail = head;
    return received_size;
}