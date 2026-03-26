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

#include "kraken_handles.h"

KRAKEN_API_BEGIN

// Port types correspond to physical ports on the board
typedef enum kraken_port_type : uint8_t {
    KRAKEN_PORT_TYPE_GPIO,
    KRAKEN_PORT_TYPE_MUX
} kraken_port_type_t;

/// Retrieves the type of the given port.
///
/// @param port The port to get the type of.
/// @param type A pointer to a variable to be populated with the type of the given port.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle or type pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_port_get_type(kraken_port_c_handle_t port, kraken_port_type_t* type);

/// Enumerates all programmable IOs exposed by the given port.
///
/// @param port The port to enumerate all IOs of.
/// @param ios A pre-allocated buffer to populate with all IO handles associated with the given port.
/// @param count A pointer to a variable that's to be populated with the number of IOs associated with the given port.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the enumeration fails.
KRAKEN_EXPORT kraken_error_t kraken_port_get_ios(kraken_port_c_handle_t port, kraken_io_handle_t* ios, size_t* count);

/// Submits any pending state changes of associated IOs to the given port.
///
/// @param port The port to update.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the update operation fails.
KRAKEN_EXPORT kraken_error_t kraken_port_update(kraken_port_handle_t port);

/// Retrieves the human-readable name of the given port.
///
/// @param port The port to retrieve the name of.
/// @param buffer A pointer to a buffer large enough to fit the given port's name.
/// @param size A pointer to a variable to be populated with the size of the given port's name in bytes.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_port_get_name(kraken_port_c_handle_t port, char* buffer, size_t* size);

/// Retrieves the human-readable name of the given port type.
///
/// @param type The port type to retrieve the name of.
/// @param name A pointer to a constant string to be populated with the name of the given port type.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given port handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_port_type_get_name(kraken_port_type_t type, const char** name);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_IO_PORT_H