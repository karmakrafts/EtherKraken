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

#include "kraken_io.h"
#include "kraken_alloc.h"
#include "kraken_error_impl.h"
#include "kraken_internal.h"
#include "kraken_io_impl.h"
#include "kraken_log_impl.h"

kraken_error_t kraken_io_create(kraken_io_t** io_addr, const char* name, const kraken_pin_config_t* pin_config,
                                const kraken_io_mode_t* supported_modes, const size_t num_supported_modes) {
    KRAKEN_CHECK_PTR(io_addr, KRAKEN_ERR_INVALID_ARG, "IO address pointer is null");
    KRAKEN_CHECK_PTR(pin_config, KRAKEN_ERR_INVALID_ARG, "Pin config pointer is null");
    KRAKEN_CHECK_PTR(supported_modes, KRAKEN_ERR_INVALID_ARG, "Supported modes pointer is null");
    KRAKEN_CHECK(num_supported_modes >= 1, KRAKEN_ERR_INVALID_ARG, "IO must support at least one mode");

    kraken_io_t* io = kraken_calloc(sizeof(kraken_io_t));
    KRAKEN_CHECK_PTR(io, KRAKEN_ERR_INVALID_OP, "Could not allocate IO instance");
    kraken_log_debug("Allocated IO at %p", (void*) io);

    memcpy(&io->pin_config, pin_config, sizeof(kraken_pin_config_t));

    char* owned_name = kraken_strdup(name);
    KRAKEN_CHECK_PTR(owned_name, KRAKEN_ERR_INVALID_OP, "Could not allocate IO name");
    io->name = owned_name;

    kraken_io_mode_t* owned_supported_modes = kraken_calloc(sizeof(kraken_io_mode_t) * num_supported_modes);
    KRAKEN_CHECK_PTR(owned_supported_modes, KRAKEN_ERR_INVALID_OP, "Could not allocate supported IO modes");
    kraken_log_debug("Allocated IO modes at %p", (void*) owned_supported_modes);
    memcpy(owned_supported_modes, supported_modes, sizeof(kraken_io_mode_t) * num_supported_modes);
    io->supported_modes = owned_supported_modes;
    io->num_supported_modes = num_supported_modes;

    *io_addr = io;
    return KRAKEN_OK;
}

kraken_error_t kraken_io_destroy(kraken_io_t* io) {
    KRAKEN_CHECK_PTR(io, KRAKEN_ERR_INVALID_ARG, "IO pointer is null");
    kraken_free(io->supported_modes);
    kraken_free(io->name);
    kraken_free(io);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_get_supported_modes(const kraken_io_c_handle_t handle, kraken_io_mode_t* modes,
                                                           size_t* count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    const kraken_io_t* io = (const kraken_io_t*) handle;
    if(modes) {
        memcpy(modes, io->supported_modes, sizeof(kraken_io_mode_t) * io->num_supported_modes);
    }
    if(count) {
        *count = io->num_supported_modes;
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_get_mode(const kraken_io_c_handle_t handle, kraken_io_mode_t* mode) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    KRAKEN_CHECK_PTR(mode, KRAKEN_ERR_INVALID_ARG, "Mode pointer is null");
    const kraken_io_t* io = (const kraken_io_t*) handle;
    *mode = io->mode;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_set_mode(kraken_io_handle_t handle, kraken_io_mode_t mode) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    kraken_io_t* io = (kraken_io_t*) handle;
    io->mode = mode;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_get(const kraken_io_c_handle_t handle, kraken_bool_t* state) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    const kraken_io_t* io = (const kraken_io_t*) handle;
    *state = io->state;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_set(kraken_io_handle_t handle, kraken_bool_t state) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    kraken_io_t* io = (kraken_io_t*) handle;
    io->state = state;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_get_name(const kraken_io_c_handle_t handle, const char** name) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    KRAKEN_CHECK_PTR(name, KRAKEN_ERR_INVALID_ARG, "Invalid name address pointer");
    const kraken_io_t* io = (const kraken_io_t*) handle;
    *name = io->name;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_io_get_pin_config(const kraken_io_c_handle_t handle,
                                                      kraken_pin_config_t* pin_config) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    KRAKEN_CHECK_PTR(pin_config, KRAKEN_ERR_INVALID_ARG, "Pin config pointer is null");
    const kraken_io_t* io = (const kraken_io_t*) handle;
    memcpy(pin_config, &io->pin_config, sizeof(kraken_pin_config_t));
    return KRAKEN_OK;
}