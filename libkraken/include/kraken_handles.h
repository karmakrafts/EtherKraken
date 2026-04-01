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

#ifndef LIBKRAKEN_KRAKEN_HANDLES_H
#define LIBKRAKEN_KRAKEN_HANDLES_H

#include "kraken_api.h"

KRAKEN_API_BEGIN

/// @brief Handle to a kraken board instance.
typedef struct kraken_board_t* kraken_board_handle_t;
/// @brief Const handle to a kraken board instance.
typedef const struct kraken_board_t* kraken_board_c_handle_t;

/// @brief Handle to a kraken port instance.
typedef struct kraken_port_t* kraken_port_handle_t;
/// @brief Const handle to a kraken port instance.
typedef const struct kraken_port_t* kraken_port_c_handle_t;

/// @brief Handle to a kraken I/O instance.
typedef struct kraken_io_t* kraken_io_handle_t;
/// @brief Const handle to a kraken I/O instance.
typedef const struct kraken_io_t* kraken_io_c_handle_t;

/// @brief Handle to a kraken flash instance.
typedef struct kraken_flash_t* kraken_flash_handle_t;
/// @brief Const handle to a kraken flash instance.
typedef const struct kraken_flash_t* kraken_flash_c_handle_t;

/// @brief Handle to a kraken dispatcher instance.
typedef struct kraken_dispatcher_t* kraken_dispatcher_handle_t;
/// @brief Const handle to a kraken dispatcher instance.
typedef const struct kraken_dispatcher_t* kraken_dispatcher_c_handle_t;

/// @brief Handle to a kraken clock instance.
typedef struct kraken_clock_t* kraken_clock_handle_t;
/// @brief Const handle to a kraken clock instance.
typedef const struct kraken_clock_t* kraken_clock_c_handle_t;

/// @brief Handle to a kraken driver instance.
typedef struct kraken_driver_t* kraken_driver_handle_t;
/// @brief Const handle to a kraken driver instance.
typedef const struct kraken_driver_t* kraken_driver_c_handle_t;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_HANDLES_H
