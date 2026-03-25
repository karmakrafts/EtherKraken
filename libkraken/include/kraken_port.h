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
    KRAKEN_PORT_EXT,
    KRAKEN_PORT_IO0,
    KRAKEN_PORT_IO1
} kraken_port_type_t;

KRAKEN_EXPORT kraken_error_t kraken_port_get_type(kraken_port_handle_t port, kraken_port_type_t* type);
KRAKEN_EXPORT kraken_error_t kraken_port_get_ios(kraken_port_handle_t port, kraken_io_handle_t* ios, size_t* count);
KRAKEN_EXPORT kraken_error_t kraken_port_update(kraken_port_handle_t port);
KRAKEN_EXPORT kraken_error_t kraken_port_get_name(kraken_port_handle_t port, char* buffer, size_t* size);
KRAKEN_EXPORT kraken_error_t kraken_port_type_get_name(kraken_port_type_t type, const char** name);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_IO_PORT_H