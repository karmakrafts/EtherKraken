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

#ifndef LIBKRAKEN_KRAKEN_DRIVER_H
#define LIBKRAKEN_KRAKEN_DRIVER_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

/// @brief Callback function that is called by the driver on every tick.
///
/// @param[in] port The port associated with the driver.
/// @param[in] user_data User-defined data passed to the driver during creation.
typedef void (*pfn_kraken_driver_tick)(kraken_port_handle_t port, void* user_data);

/// @brief Creates a new driver instance.
///
/// @param[in] port The port associated with the driver.
/// @param[in] callback The callback function that is called by the driver on every tick.
/// @param[in] user_data User-defined data to be passed to the callback function.
/// @param[out] handle A pointer to a variable that is to be populated with the handle to the new driver.
/// @return @code KRAKEN_OK @endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG @endcode when any of the required parameters are invalid.
KRAKEN_EXPORT kraken_error_t kraken_driver_create(kraken_port_handle_t port, pfn_kraken_driver_tick callback,
                                                  void* user_data, kraken_driver_handle_t* handle);

/// @brief Destroys the given driver instance.
///
/// @param[in] handle The handle of the driver to destroy.
/// @return @code KRAKEN_OK @endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG @endcode when the given driver handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_driver_destroy(kraken_driver_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_DRIVER_H