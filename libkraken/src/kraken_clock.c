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

#include "driver/kraken_driver_impl.h"
#include "kraken_clock_impl.h"
#include "kraken_cpu.h"
#include "kraken_error_impl.h"

KRAKEN_EXPORT kraken_error_t kraken_clock_create(const double frequency, kraken_clock_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid clock handle pointer");
    KRAKEN_CHECK(frequency >= 0.5, KRAKEN_ERR_INVALID_ARG, "Clock frequency must be >= 0.5Hz");
    kraken_clock_t* clock = kraken_malloc(sizeof(kraken_clock_t));

    KRAKEN_CHECK_CALL_ERR(kraken_array_list_create(4, sizeof(kraken_driver_handle_t), &clock->drivers),
                          "Could not allocate clock driver list");

    KRAKEN_CHECK_CALL_RES(pthread_mutex_init(&clock->drivers_mutex, nullptr), KRAKEN_ERR_INVALID_OP,
                          "Could not initialize clock driver list mutex");

    clock->frequency = frequency;

    uint64_t system_frequency = 0;
    KRAKEN_CHECK_CALL_ERR(kraken_cpu_get_system_frequency(&system_frequency), "Could not retrieve system frequency");
    const uint64_t period = (uint64_t) ((double) system_frequency / frequency + 0.5);
    clock->period = period;

    uint64_t now = 0;
    KRAKEN_CHECK_CALL_ERR(kraken_cpu_get_counter(&now), "Could not retrieve system counter");
    clock->next_event = now - period;

    *handle = clock;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_clock_register(kraken_clock_handle_t handle, kraken_driver_handle_t driver) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid clock handle");
    KRAKEN_CHECK_PTR(driver, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    pthread_mutex_lock(&handle->drivers_mutex);
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_add(&handle->drivers, &driver),
                          "Could not add driver to clock drivers list");
    pthread_mutex_unlock(&handle->drivers_mutex);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_clock_unregister(kraken_clock_handle_t handle, kraken_driver_handle_t driver) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid clock handle");
    KRAKEN_CHECK_PTR(driver, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    pthread_mutex_lock(&handle->drivers_mutex);
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_remove(&handle->drivers, &driver),
                          "Could not remove driver from clock drivers");
    pthread_mutex_unlock(&handle->drivers_mutex);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_clock_destroy(kraken_clock_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid clock handle");
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_destroy(&handle->drivers), "Could not destroy clock drivers list");
    KRAKEN_CHECK_CALL_RES(pthread_mutex_destroy(&handle->drivers_mutex), KRAKEN_ERR_INVALID_OP,
                          "Could not destroy clock drivers list mutex");
    free(handle);
    return KRAKEN_OK;
}

kraken_error_t kraken_clock_tick(kraken_clock_t* clock) {
    pthread_mutex_lock(&clock->drivers_mutex);
    for(size_t index = 0; index < clock->drivers.size; index++) {
        kraken_driver_t* driver = nullptr;
        KRAKEN_CHECK_CALL_ERR(kraken_array_list_get(&clock->drivers, index, &driver),
                              "Could not retrieve clock driver");
        // TODO: implement update voting
        kraken_port_t* port = driver->port;
        const uint64_t io_mask = driver->pfn_tick(port, driver->user_data);
        kraken_port_update_masked(port, io_mask);
    }
    pthread_mutex_unlock(&clock->drivers_mutex);
    return KRAKEN_OK;
}