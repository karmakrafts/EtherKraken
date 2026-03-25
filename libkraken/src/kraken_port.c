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

#include "kraken_error.h"
#include "kraken_error_impl.h"
#include "kraken_port_impl.h"

KRAKEN_EXPORT kraken_error_t kraken_port_get_type(kraken_port_handle_t port, kraken_port_type_t* type) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Port handle is null");
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_get_ios(kraken_port_handle_t port, kraken_io_handle_t* ios, size_t* count) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Port handle is null");
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_update(kraken_port_handle_t port) {
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_get_name(kraken_port_handle_t port, char* buffer, size_t* size) {
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_type_get_name(kraken_port_type_t type, const char** name) {
    KRAKEN_CHECK_PTR(name, KRAKEN_ERR_INVALID_ARG, "Name pointer is null");
    switch(type) {// clang-format off
        case KRAKEN_PORT_EXT:
            *name = "EXT";
            break;
        case KRAKEN_PORT_IO0:
            *name = "IO0";
            break;
        case KRAKEN_PORT_IO1:
            *name = "IO2";
            break;
        default:
            *name = "Unknown";
            break;
    }// clang-format on
    return KRAKEN_OK;
}