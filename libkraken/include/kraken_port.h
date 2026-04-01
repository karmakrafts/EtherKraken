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

#ifndef LIBKRAKEN_KRAKEN_IO_PORT_H
#define LIBKRAKEN_KRAKEN_IO_PORT_H

#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

/// @brief Represents the type of a port.
typedef enum kraken_port_type : uint8_t {
    KRAKEN_PORT_TYPE_GPIO,   ///< A general purpose IO port.
    KRAKEN_PORT_TYPE_I2C_MUX,///< An I2C multiplexer port.
    KRAKEN_PORT_TYPE_SPI_MUX ///< A SPI multiplexer port.
} kraken_port_type_t;

/// @brief Retrieves the type of the given port.
///
/// @param[in] handle The port to get the type of.
/// @param[out] type A pointer to a variable to be populated with the type of the given port.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle or type pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_port_get_type(kraken_port_c_handle_t handle, kraken_port_type_t* type);

/// @brief Enumerates all programmable IOs exposed by the given port.
///
/// @param[in] handle The port to enumerate all IOs of.
/// @param[out] ios A pre-allocated buffer to populate with all IO handles associated with the given port.
/// @param[out] count A pointer to a variable that's to be populated with the number of IOs associated with the given port.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the enumeration fails.
KRAKEN_EXPORT kraken_error_t kraken_port_get_ios(kraken_port_c_handle_t handle, kraken_io_handle_t* ios, size_t* count);

/// @brief Submits any pending state changes of associated IOs to the given port.
///
/// @param[in] handle The port to update.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the update operation fails.
KRAKEN_EXPORT kraken_error_t kraken_port_update(kraken_port_c_handle_t handle);

/// @brief Submits state changes of associated IOs to the given port using a mask.
///
/// @param[in] handle The port to update.
/// @param[in] mask The mask of IOs to update.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the update operation fails.
KRAKEN_EXPORT kraken_error_t kraken_port_update_masked(kraken_port_c_handle_t handle, uint64_t mask);

/// @brief Re-initializes the port to reflect any changes to the mode of the associated IOs.
///
/// @param[in] handle The port to re-initialize.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the update operation fails.
KRAKEN_EXPORT kraken_error_t kraken_port_reinit(kraken_port_c_handle_t handle);

/// @brief Retrieves the name of the given port.
///
/// @param[in] handle The port to get the name of.
/// @param[out] name A pointer to a constant string to be populated with the name of the given port.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle or name pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_port_get_name(kraken_port_c_handle_t handle, const char** name);

/// @brief Retrieves the human-readable name of the given port type.
///
/// @param[in] type The port type to retrieve the name of.
/// @param[out] name A pointer to a constant string to be populated with the name of the given port type.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_port_type_get_name(kraken_port_type_t type, const char** name);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_IO_PORT_H