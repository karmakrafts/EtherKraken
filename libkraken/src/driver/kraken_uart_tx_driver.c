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

static uint64_t kraken_uart_tx_driver_tick(kraken_port_handle_t port, void* user_data) {
    kraken_uart_tx_state_t* state = user_data;

    return KRAKEN_DRIVER_IO_MASK_NONE;
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
    state->frame_size = (int8_t) (2 + config->data_bits + config->stop_bits);
    state->last_bit = (int8_t) (state->frame_size - 1);

    // Find the matching IO
    for(size_t index = 0; index < port->num_ios; index++) {
        kraken_io_t* io = port->ios[index];
        if(io->pin_config.device_pin != pin) {
            continue;
        }
        state->io = io;
    }
    KRAKEN_CHECK_PTR(state->io, KRAKEN_ERR_INVALID_OP, "Could not find matching IO for UART TX driver");

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
    while(atomic_load(&state->bit) < state->last_bit) {
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