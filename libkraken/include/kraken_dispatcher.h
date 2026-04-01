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

#ifndef LIBKRAKEN_KRAKEN_DISPATCHER_HPP
#define LIBKRAKEN_KRAKEN_DISPATCHER_HPP

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

/// @brief Creates a new dispatcher.
///
/// @param[in] core_affinity The CPU core affinity for the dispatcher thread.
/// @param[out] handle A pointer to a handle to be populated with the new dispatcher.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle pointer is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the creation fails.
KRAKEN_EXPORT kraken_error_t kraken_dispatcher_create(int32_t core_affinity, kraken_dispatcher_handle_t* handle);

/// @brief Registers a clock with the given dispatcher.
///
/// @param[in] handle The dispatcher to register the clock with.
/// @param[in] clock The clock to register.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given dispatcher or clock handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the registration fails.
KRAKEN_EXPORT kraken_error_t kraken_dispatcher_register(kraken_dispatcher_handle_t handle, kraken_clock_handle_t clock);

/// @brief Unregisters a clock from the given dispatcher.
///
/// @param[in] handle The dispatcher to unregister the clock from.
/// @param[in] clock The clock to unregister.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given dispatcher or clock handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the unregistration fails.
KRAKEN_EXPORT kraken_error_t kraken_dispatcher_unregister(kraken_dispatcher_handle_t handle,
                                                          kraken_clock_handle_t clock);

/// @brief Retrieves all clocks registered with the given dispatcher.
///
/// @param[in] handle The dispatcher to get the clocks from.
/// @param[out] clocks A pre-allocated buffer to populate with all clock handles associated with the given dispatcher.
/// @param[out] count A pointer to a variable that's to be populated with the number of clocks associated with the given dispatcher.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given dispatcher handle or count pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_dispatcher_get_clocks(kraken_dispatcher_handle_t handle,
                                                          kraken_clock_handle_t* clocks, size_t* count);

/// @brief Destroys the given dispatcher.
///
/// @param[in] handle The dispatcher to destroy.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given dispatcher handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_dispatcher_destroy(kraken_dispatcher_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_DISPATCHER_HPP
