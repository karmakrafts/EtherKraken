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

#include "driver/kraken_driver.h"
#include "kraken_driver_impl.h"
#include "kraken_error_impl.h"
#include "kraken_uart_tx_driver_impl.h"
#include "port/kraken_port_impl.h"

// Standard UART at 0-3.3V logic levels.
// See https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter.
//
//   3.3V (Logic 1) ┠────┐   ┌───┐       ┌───────────┐   ┌────────────────┨
//                  ┃    │   │   │       │           │   │                ┃
//     0V (Logic 0) ┃    └───┘   └───────┘           └───┘                ┃
//                  ┃ IDL STA DA0 DA1 DA2 DA3 DA4 DA5 DA6 DA7 PAR STP IDL ┃
//
// The idle state of the transmission wire is a logic high at 3.3V.
// The data bits are configurable from 5-8 and parity bit is optional.

static uint64_t kraken_uart_tx_driver_tick(kraken_port_handle_t, void* user_data) {
    if(!user_data) {
        return KRAKEN_DRIVER_IO_MASK_NONE;
    }
    kraken_uart_tx_state_t* state = user_data;

    uint8_t current_byte = atomic_load(&state->byte);
    int8_t current_bit = atomic_load(&state->bit);
    if(current_bit == KRAKEN_UART_TX_STOP_BIT || current_bit == state->last_stop_bit) {
        bool is_empty = false;
        if(kraken_byte_queue_is_empty(state->buffer, &is_empty) != KRAKEN_OK || is_empty) {
            return KRAKEN_DRIVER_IO_MASK_NONE;// No data, no IO gets updated
        }
        if(kraken_byte_queue_dequeue(state->buffer, &current_byte) != KRAKEN_OK) {
            return KRAKEN_DRIVER_IO_MASK_NONE;// Data dequeue failed, no IO gets updated
        }
        atomic_store(&state->byte, current_byte);
        current_bit = state->start_bit;
        atomic_store(&state->bit, current_bit);// Reset to start bit
    }

    if(current_bit == 0) {// Start bit, always logic low
        atomic_store(&state->io->state, KRAKEN_FALSE);
    }
    else if(current_bit >= state->first_data_bit && current_bit <= state->last_data_bit) {// Data bits
        const int32_t bit_index = current_bit - state->first_data_bit;
        const kraken_bool_t bit_value = (kraken_bool_t) (current_byte >> bit_index & 0b1);
        atomic_store(&state->io->state, bit_value);
    }
    else if(state->parity_bit != KRAKEN_UART_TX_NO_PARITY && current_bit == state->parity_bit) {
        const int32_t count = __builtin_popcount(state->byte);
        kraken_bool_t bit_value = KRAKEN_FALSE;
        switch(state->config->parity) {
            case KRAKEN_UART_PARITY_ODD: {
                bit_value = (kraken_bool_t) ((count & 0b1) == 0b1);
                break;
            }
            case KRAKEN_UART_PARITY_EVEN: {
                bit_value = (kraken_bool_t) ((count & 0b1) == 0b0);
                break;
            }
            case KRAKEN_UART_PARITY_MARK: {
                bit_value = KRAKEN_TRUE;
                break;
            }
            default: break;
        }
        atomic_store(&state->io->state, bit_value);
    }
    else if(current_bit >= state->first_stop_bit && current_bit <= state->last_stop_bit) {
        atomic_store(&state->io->state, KRAKEN_TRUE);// Stop bits, always logic high
    }

    atomic_store(&state->bit, current_bit + 1);
    return state->io_mask;
}

KRAKEN_EXPORT kraken_error_t kraken_uart_tx_driver_create(const kraken_uart_config_t* config, kraken_port_handle_t port,
                                                          kraken_driver_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle pointer");

    kraken_uart_tx_state_t* state = kraken_calloc(sizeof(kraken_uart_tx_state_t));
    KRAKEN_CHECK_PTR(state, KRAKEN_ERR_INVALID_OP, "Could not allocate UART TX driver state");
    state->config = kraken_heapcopy_of(config);
    const uint32_t pin = config->pin;
    state->io_mask = KRAKEN_DRIVER_IO_MASK(pin);
    atomic_store(&state->bit, KRAKEN_UART_TX_STOP_BIT);

    // Start bit + parity bit + data bits + stop bits
    state->frame_size = (int8_t) (1 + config->data_bits + config->stop_bits);
    if(config->parity != KRAKEN_UART_PARITY_NONE) {
        state->frame_size++;// Additional parity bit if parity is enabled
    }
    const bool has_parity = config->parity != KRAKEN_UART_PARITY_NONE;
    state->start_bit = 0;// Start bit is always index 0
    state->first_data_bit = 1;
    state->last_data_bit = (int8_t) (config->data_bits + 1);
    state->parity_bit = has_parity ? (int8_t) (state->last_data_bit + 1) : -1;
    state->first_stop_bit = (int8_t) (has_parity ? state->parity_bit + 1 : state->last_data_bit + 1);
    state->last_stop_bit = (int8_t) (state->first_stop_bit + config->stop_bits);

    // Find the matching IO
    for(size_t index = 0; index < port->num_ios; index++) {
        kraken_io_t* io = port->ios[index];
        if(io->pin_config.device_pin != pin) {
            continue;
        }
        state->io = io;
    }
    KRAKEN_CHECK_PTR(state->io, KRAKEN_ERR_INVALID_OP, "Could not find matching IO for UART TX driver");

    atomic_store(&state->io->pud_mode, KRAKEN_IO_PUD_MODE_UP);
    KRAKEN_CHECK_CALL_ERR(kraken_port_reinit(port), "Could not initialize UART TX driver port state");

    atomic_store(&state->io->state, KRAKEN_TRUE);// Idle state is high (stop bit)
    KRAKEN_CHECK_CALL_ERR(kraken_port_update_masked(port, state->io_mask),
                          "Could not initialize UART TX driver port state");

    usleep(1000);// Wait a small moment to reach a stable state

    KRAKEN_CHECK_CALL_ERR(kraken_byte_queue_create(config->buffer_size, &state->buffer),
                          "Could not initialize serial TX queue");

    kraken_driver_handle_t driver = nullptr;
    KRAKEN_CHECK_CALL_ERR(kraken_driver_create(port, &kraken_uart_tx_driver_tick, state, &driver),
                          "Could not create UART TX driver instance");
    *handle = driver;

    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_uart_tx_driver_get_config(kraken_driver_c_handle_t handle,
                                                              kraken_uart_config_t** config) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid driver config address pointer");
    const kraken_uart_tx_state_t* state = handle->user_data;
    *config = state->config;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_uart_tx_driver_write(kraken_driver_handle_t handle, const void* words,
                                                         size_t word_count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    KRAKEN_CHECK_PTR(words, KRAKEN_ERR_INVALID_ARG, "Invalid words pointer");
    KRAKEN_CHECK(word_count > 0, KRAKEN_ERR_INVALID_ARG, "Word count for serial TX driver must be > 0");
    kraken_uart_tx_state_t* state = handle->user_data;
    for(size_t index = 0; index < word_count; index++) {
        const uint8_t* source_addr = (const uint8_t*) words + index;
        KRAKEN_CHECK_CALL_ERR(kraken_byte_queue_enqueue(state->buffer, *source_addr),
                              "Could not enqueue data word to serial TX driver");
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_uart_tx_driver_wait(kraken_driver_c_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    const kraken_uart_tx_state_t* state = handle->user_data;
    bool is_empty = false;
    KRAKEN_CHECK_CALL_ERR(kraken_byte_queue_is_empty(state->buffer, &is_empty),
                          "Could not check serial TX driver buffer state");
    // Wait until bytes are consumed
    while(!is_empty) {
        sched_yield();// Waiting for a driver assumed we call from preemptive code into RT state
        KRAKEN_CHECK_CALL_ERR(kraken_byte_queue_is_empty(state->buffer, &is_empty),
                              "Could not check serial TX driver buffer state");
    }
    // Wait until we reached last bit
    while(atomic_load(&state->bit) < state->last_stop_bit) {
        sched_yield();// Waiting for a driver assumed we call from preemptive code into RT state
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_uart_tx_driver_destroy(kraken_driver_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid serial TX driver handle");
    kraken_uart_tx_state_t* state = handle->user_data;
    kraken_byte_queue_destroy(state->buffer);
    kraken_free(state->config);
    kraken_free(state);
    handle->user_data = nullptr;
    kraken_driver_destroy(handle);
    return KRAKEN_OK;
}