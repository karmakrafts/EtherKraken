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

#ifndef LIBKRAKEN_KRAKEN_PIN_H
#define LIBKRAKEN_KRAKEN_PIN_H

#include "config/kraken_config.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

/// @brief I/O mode for a kraken I/O pin.
typedef enum kraken_io_mode : uint8_t {
    KRAKEN_IO_MODE_IN,///< Input mode.
    KRAKEN_IO_MODE_OUT///< Output mode.
} kraken_io_mode_t;

/// @brief Retrieves the supported I/O modes for a given I/O handle.
///
/// @param[in] handle The I/O handle to query.
/// @param[out] modes Pointer to an array to store the supported modes.
/// @param[in,out] count Pointer to the number of elements in the modes array.
///                      On input, should contain the size of the modes array.
///                      On output, contains the number of supported modes.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_get_supported_modes(kraken_io_c_handle_t handle, kraken_io_mode_t* modes,
                                                           size_t* count);

/// @brief Retrieves the current I/O mode of a given I/O handle.
///
/// @param[in] handle The I/O handle to query.
/// @param[out] mode Pointer to store the current I/O mode.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_get_mode(kraken_io_c_handle_t handle, kraken_io_mode_t* mode);

/// @brief Sets the I/O mode for a given I/O handle.
///
/// @param[in] io The I/O handle to modify.
/// @param[in] mode The new I/O mode to set.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_set_mode(kraken_io_handle_t io, kraken_io_mode_t mode);

/// @brief Retrieves the current state of a given I/O handle.
///
/// @param[in] io The I/O handle to query.
/// @param[out] state Pointer to store the current state (KRAKEN_TRUE or KRAKEN_FALSE).
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_get(kraken_io_c_handle_t io, kraken_bool_t* state);

/// @brief Sets the state of a given I/O handle.
///
/// @param[in] io The I/O handle to modify.
/// @param[in] state The new state to set (KRAKEN_TRUE or KRAKEN_FALSE).
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_set(kraken_io_handle_t io, kraken_bool_t state);

/// @brief Retrieves the name of a given I/O handle.
///
/// @param[in] io The I/O handle to query.
/// @param[out] name Pointer to store the name string. The string is owned by the library.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_get_name(kraken_io_c_handle_t io, const char** name);

/// @brief Retrieves the pin configuration for a given I/O handle.
///
/// @param[in] io The I/O handle to query.
/// @param[out] config Pointer to store the pin configuration.
/// @return KRAKEN_SUCCESS if successful, or an error code otherwise.
KRAKEN_EXPORT kraken_error_t kraken_io_get_pin_config(kraken_io_c_handle_t io, kraken_pin_config_t* config);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_PIN_H
