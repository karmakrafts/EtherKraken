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

#ifndef LIBKRAKEN_KRAKEN_CLOCK_H
#define LIBKRAKEN_KRAKEN_CLOCK_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

/// @brief Creates a new clock.
///
/// @param[in] frequency The clock frequency in Hz.
/// @param[out] handle A pointer to a handle to be populated with the new clock.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle pointer is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the creation fails.
KRAKEN_EXPORT kraken_error_t kraken_clock_create(double frequency, kraken_clock_handle_t* handle);

/// @brief Registers a driver with the given clock.
///
/// @param[in] handle The clock to register the driver with.
/// @param[in] driver The driver to register.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given clock or driver handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the registration fails.
KRAKEN_EXPORT kraken_error_t kraken_clock_register(kraken_clock_handle_t handle, kraken_driver_handle_t driver);

/// @brief Unregisters a driver from the given clock.
///
/// @param[in] handle The clock to unregister the driver from.
/// @param[in] driver The driver to unregister.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given clock or driver handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the unregistration fails.
KRAKEN_EXPORT kraken_error_t kraken_clock_unregister(kraken_clock_handle_t handle, kraken_driver_handle_t driver);

/// @brief Destroys the given clock.
///
/// @param[in] handle The clock to destroy.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given clock handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_clock_destroy(kraken_clock_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_CLOCK_H