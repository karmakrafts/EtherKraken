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

#include "kraken_driver_impl.h"
#include "kraken_error_impl.h"
#include "kraken_serial_tx_driver_impl.h"

static uint64_t kraken_serial_tx_driver_tick(kraken_port_handle_t port, void* user_data) {
    if(!user_data) {
        return KRAKEN_DRIVER_IO_MASK_NONE;
    }
    kraken_serial_tx_state_t* state = user_data;
    // If we are at the last- or stop-bit, we need to fetch a new byte from the buffer
    uint8_t current_byte = atomic_load(&state->byte);
    const int8_t current_bit = atomic_load(&state->bit);
    if(current_bit == KRAKEN_SERIAL_TX_STOP_BIT || current_bit == state->last_bit) {
        bool is_empty = false;
        if(kraken_byte_queue_is_empty(state->buffer, &is_empty) != KRAKEN_OK || is_empty) {
            return KRAKEN_DRIVER_IO_MASK_NONE;// No data, no IO gets updated
        }
        if(kraken_byte_queue_dequeue(state->buffer, &current_byte) != KRAKEN_OK) {
            kraken_panic("Could not dequeue data from serial TX buffer");
        }
        atomic_store(&state->byte, current_byte);
        atomic_store(&state->bit, 0);
    }
    // Update clock state
    const kraken_bool_t clock_state = atomic_load(&state->clock_io->state);
    atomic_store(&state->clock_io->state, !clock_state);
    // Update data state and increment bit counter
    atomic_store(&state->data_io->state, current_byte >> state->bit++ & 0b1U);
    return state->io_mask;
}

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_create(const kraken_serial_tx_config_t* config,
                                                            kraken_port_handle_t port, kraken_driver_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle pointer");

    kraken_serial_tx_state_t* state = kraken_calloc(sizeof(kraken_serial_tx_state_t));
    KRAKEN_CHECK_PTR(state, KRAKEN_ERR_INVALID_OP, "Could not allocate serial TX driver state");
    state->config = kraken_heapcopy_of(config);
    state->io_mask = KRAKEN_DRIVER_IO_MASK(config->clock_pin) | KRAKEN_DRIVER_IO_MASK(config->data_pin);
    atomic_store(&state->bit, KRAKEN_SERIAL_TX_STOP_BIT);
    state->last_bit = (int8_t) (config->word_size - 1);

    // Find the right IOs and cache their address
    for(size_t index = 0; index < port->num_ios; index++) {
        kraken_io_t* io = port->ios[index];
        const uint32_t device_pin = io->pin_config.device_pin;
        if(device_pin == config->clock_pin) {
            state->clock_io = io;
        }
        else if(device_pin == config->data_pin) {
            state->data_io = io;
        }
    }

    KRAKEN_CHECK_CALL_ERR(kraken_byte_queue_create(config->buffer_size, &state->buffer),
                          "Could not initialize serial TX queue");

    kraken_driver_handle_t driver = nullptr;
    KRAKEN_CHECK_CALL_ERR(kraken_driver_create(port, &kraken_serial_tx_driver_tick, state, &driver),
                          "Could not create serial TX driver instance");

    *handle = driver;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_get_config(const kraken_driver_c_handle_t handle,
                                                                kraken_serial_tx_config_t** config) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid driver config address pointer");
    const kraken_serial_tx_state_t* state = handle->user_data;
    *config = state->config;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_write(kraken_driver_handle_t handle, const void* words,
                                                           const size_t word_count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    KRAKEN_CHECK_PTR(words, KRAKEN_ERR_INVALID_ARG, "Invalid words pointer");
    KRAKEN_CHECK(word_count > 0, KRAKEN_ERR_INVALID_ARG, "Word count for serial TX driver must be > 0");
    kraken_serial_tx_state_t* state = handle->user_data;
    for(size_t index = 0; index < word_count; index++) {
        const uint8_t* source_addr = (const uint8_t*) words + index;
        KRAKEN_CHECK_CALL_ERR(kraken_byte_queue_enqueue(state->buffer, *source_addr),
                              "Could not enqueue data word to serial TX driver");
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_wait(const kraken_driver_c_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    const kraken_serial_tx_state_t* state = handle->user_data;
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

KRAKEN_EXPORT kraken_error_t kraken_serial_tx_driver_destroy(kraken_driver_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid serial TX driver handle");
    kraken_serial_tx_state_t* state = handle->user_data;
    kraken_byte_queue_destroy(state->buffer);
    kraken_free(state->config);
    kraken_free(state);
    handle->user_data = nullptr;
    kraken_driver_destroy(handle);
    return KRAKEN_OK;
}