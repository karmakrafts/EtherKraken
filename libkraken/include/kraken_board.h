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

#ifndef LIBKRAKEN_KRAKEN_BOARD_H
#define LIBKRAKEN_KRAKEN_BOARD_H

#include "kraken_api.h"
#include "kraken_config.h"
#include "kraken_error.h"
#include "kraken_handles.h"
#include "kraken_port.h"

KRAKEN_API_BEGIN

typedef enum kraken_board_type : uint8_t {
    KRAKEN_BOARD_TYPE_1B// EtherKraken V1 Rev. B
} kraken_board_type_t;

/// Initializes the given configuration with the defaults for the specified board.
///
/// @param type The board type to initialize the config for.
/// @param config A pointer to a board config to populate with the defaults for the given board type.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given config pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_config_init(kraken_board_type_t type, kraken_board_config_t* config);

/// Creates a new board instance from the given configuration and initializes
/// all configured hardware.
///
/// @param config The configuration to create a new board instance from.
/// @param handle A pointer to a variable to be populated with a handle to the newly created board.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given config pointer or handle pointer are invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the config is invalid or board initialization fails.
KRAKEN_EXPORT kraken_error_t kraken_board_create(const kraken_board_config_t* config, kraken_board_handle_t* handle);

KRAKEN_EXPORT kraken_error_t kraken_board_get_config(kraken_board_c_handle_t handle,
                                                     const kraken_board_config_t** config);

/// Enumerates the ports associated with the given board.
///
/// @param handle The board to enumerate all ports of.
/// @param ports A pointer to a pre-allocated buffer to be populated with all ports associated with the given board.
///     May be @code nullptr@endcode if not needed.
/// @param count A pointer to a variable to be populated with the number of ports associated with the given board.
///     May be @code nullptr@endcode if not needed.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_get_ports(kraken_board_c_handle_t handle, kraken_port_handle_t* ports,
                                                    size_t* count);

/// Retrieves the flash device associated with the given board.
///
/// @param handle The board to retrieve the flash memory of.
/// @param flash A pointer to variable to be populated with the handle of the flash associated with the given board.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle or pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_get_flash(kraken_board_c_handle_t handle, kraken_flash_handle_t* flash);

KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle);

KRAKEN_EXPORT kraken_error_t kraken_board_get_port_for_io(kraken_board_c_handle_t handle, kraken_io_c_handle_t io,
                                                          kraken_port_handle_t* port);

KRAKEN_EXPORT kraken_error_t kraken_board_get_ports_for_type(kraken_board_c_handle_t handle, kraken_port_type_t type,
                                                             kraken_port_handle_t* ports, size_t* count);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_BOARD_H
