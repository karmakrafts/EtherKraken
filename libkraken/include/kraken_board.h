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

#include "config/kraken_config.h"
#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"
#include "kraken_port.h"

KRAKEN_API_BEGIN

/// @brief Represents the different types of EtherKraken boards supported by the library.
typedef enum kraken_board_type : uint8_t {
    KRAKEN_BOARD_TYPE_1B = 0///< EtherKraken V1 Rev. B
} kraken_board_type_t;

/// @brief Initializes the given configuration with the defaults for the specified board.
///
/// @param[in] type The board type to initialize the config for.
/// @param[out] config A pointer to a board config to populate with the defaults for the given board type.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given config pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_config_init(kraken_board_type_t type, kraken_board_config_t* config);

/// @brief Creates a new board instance from the given configuration and initializes
/// all configured hardware.
///
/// @param[in] config The configuration to create a new board instance from.
/// @param[out] handle A pointer to a variable to be populated with a handle to the newly created board.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given config pointer or handle pointer are invalid,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the config is invalid or board initialization fails.
KRAKEN_EXPORT kraken_error_t kraken_board_create(const kraken_board_config_t* config, kraken_board_handle_t* handle);

/// @brief Retrieves the configuration associated with the given board.
///
/// @param[in] handle The board to retrieve the configuration of.
/// @param[out] config A pointer to a variable to be populated with the configuration of the given board.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle or pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_get_config(kraken_board_c_handle_t handle,
                                                     const kraken_board_config_t** config);

/// @brief Enumerates the ports associated with the given board.
///
/// @param[in] handle The board to enumerate all ports of.
/// @param[out] ports A pointer to a pre-allocated buffer to be populated with all ports associated with the given board.
///     May be @code nullptr@endcode if not needed.
/// @param[out] count A pointer to a variable to be populated with the number of ports associated with the given board.
///     May be @code nullptr@endcode if not needed.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_get_ports(kraken_board_c_handle_t handle, kraken_port_handle_t* ports,
                                                    size_t* count);

/// @brief Retrieves the flash device associated with the given board.
///
/// @param[in] handle The board to retrieve the flash memory of.
/// @param[out] flash A pointer to variable to be populated with the handle of the flash associated with the given board.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle or pointer is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_get_flash(kraken_board_c_handle_t handle, kraken_flash_handle_t* flash);

/// @brief Destroys the given board instance and releases all associated resources.
///
/// @param[in] handle The board instance to destroy.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_OP@endcode when the given handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle);

/// @brief Retrieves the port associated with the given IO.
///
/// @param[in] handle The board to search for the port in.
/// @param[in] io The IO to retrieve the associated port for.
/// @param[out] port A pointer to a variable to be populated with the handle of the port associated with the given IO.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle, IO, or port pointer is invalid, or
///     if no port is associated with the given IO.
KRAKEN_EXPORT kraken_error_t kraken_board_get_port_for_io(kraken_board_c_handle_t handle, kraken_io_c_handle_t io,
                                                          kraken_port_handle_t* port);

/// @brief Retrieves all ports of the given type associated with the given board.
///
/// @param[in] handle The board to retrieve all ports of the given type of.
/// @param[in] type The type of ports to retrieve.
/// @param[out] ports A pointer to a pre-allocated buffer to be populated with all ports of the given type.
///     May be @code nullptr@endcode if not needed.
/// @param[in,out] count A pointer to a variable to be populated with the number of ports of the given type.
///     If @code ports@endcode is not @code nullptr@endcode, this variable must contain the maximum
///     number of ports to be copied into the buffer.
///     May be @code nullptr@endcode if not needed.
/// @return @code KRAKEN_OK@endcode when successful,
///     @code KRAKEN_ERR_INVALID_ARG@endcode when the given handle is invalid.
KRAKEN_EXPORT kraken_error_t kraken_board_get_ports_for_type(kraken_board_c_handle_t handle, kraken_port_type_t type,
                                                             kraken_port_handle_t* ports, size_t* count);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_BOARD_H
