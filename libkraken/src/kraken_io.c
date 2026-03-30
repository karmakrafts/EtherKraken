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

    kraken_io_t* io = kraken_alloc(kraken_io_t);
    memset(io, 0x00, sizeof(kraken_io_t));
    kraken_log_debug("Allocated IO at %p", (void*) io);
    KRAKEN_CHECK_PTR(io, KRAKEN_ERR_INVALID_OP, "Could not allocate IO instance");

    memcpy(&io->pin_config, pin_config, sizeof(kraken_pin_config_t));

    const size_t name_size = strlen(name) + 1;
    kraken_log_debug("Allocating %zu bytes of IO name memory", name_size);
    char* owned_name = malloc(name_size);
    KRAKEN_CHECK_PTR(owned_name, KRAKEN_ERR_INVALID_OP, "Could not allocate IO name");
    kraken_log_debug("Allocated IO name at %p", (void*) owned_name);
    memcpy(owned_name, name, name_size);
    io->name = owned_name;

    kraken_io_mode_t* owned_supported_modes = kraken_alloc_array(kraken_io_mode_t, num_supported_modes);
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
    free(io->supported_modes);
    free(io->name);
    free(io);
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

KRAKEN_EXPORT kraken_error_t kraken_io_get_name(const kraken_io_c_handle_t handle, char* buffer, size_t* size) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    const kraken_io_t* io = (const kraken_io_t*) handle;
    if(buffer) {
        if(size) {
            memcpy(buffer, io->name, *size);
            return KRAKEN_OK;
        }
        memcpy(buffer, io->name, strlen(io->name));
        return KRAKEN_OK;
    }
    KRAKEN_CHECK_PTR(size, KRAKEN_ERR_INVALID_ARG, "Size pointer cannot be null when buffer is null");
    *size = strlen(io->name);// NOLINT
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