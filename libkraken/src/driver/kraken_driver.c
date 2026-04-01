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

KRAKEN_EXPORT kraken_error_t kraken_driver_create(kraken_port_handle_t port, pfn_kraken_driver_tick callback,
                                                  void* user_data, kraken_driver_handle_t* handle) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port handle");
    KRAKEN_CHECK_PTR(callback, KRAKEN_ERR_INVALID_ARG, "Invalid driver callback");
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle pointer");
    kraken_driver_t* driver = kraken_malloc(sizeof(kraken_driver_t));
    driver->user_data = user_data;
    driver->pfn_tick = callback;
    driver->port = (kraken_port_t*) port;
    *handle = (kraken_driver_handle_t) driver;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_driver_destroy(kraken_driver_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid driver handle");
    kraken_driver_t* driver = (kraken_driver_t*) handle;
    free(driver);
    return KRAKEN_OK;
}