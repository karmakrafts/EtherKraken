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

#include "libkraken.h"

#include "config/etherkraken_v1b.h"
#include "config/kraken_config_impl.h"
#include "kraken_board_impl.h"
#include "kraken_error_impl.h"
#include "kraken_flash_impl.h"
#include "kraken_internal.h"
#include "kraken_log_impl.h"
#include "port/kraken_gpio_port.h"
#include "port/kraken_i2c_mux_port.h"
#include "port/kraken_spi_mux_port.h"

#include <sys/ioctl.h>

KRAKEN_EXPORT kraken_error_t kraken_board_config_init(const kraken_board_type_t type, kraken_board_config_t* config) {
    switch(type) {
        case KRAKEN_BOARD_TYPE_1B: {
            memcpy(config, &g_config_v1b, sizeof(kraken_board_config_t));
            return KRAKEN_OK;
        }
        default: return KRAKEN_ERR_INVALID_ARG;
    }
}

KRAKEN_EXPORT kraken_error_t kraken_board_create(const kraken_board_config_t* config, kraken_board_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Board handle pointer is null");
    kraken_board_t* board = kraken_calloc(sizeof(kraken_board_t));
    KRAKEN_CHECK_PTR(board, KRAKEN_ERR_INVALID_OP, "Could not allocate board instance");

    kraken_log_debug("Copying board config to board instance");
    KRAKEN_CHECK_CALL_ERR(kraken_board_config_copy(config, &board->config),
                          "Could not copy board config to board instance");

    // Calculate and update port count
    const size_t num_ports = board->config->mux_count + 1;
    board->num_ports = num_ports;
    kraken_log_debug("Allocating memory for %zu ports", num_ports);

    kraken_port_t** ports = kraken_calloc(sizeof(kraken_port_t*) * num_ports);
    KRAKEN_CHECK_PTR(ports, KRAKEN_ERR_INVALID_OP, "Could not allocate IO ports");
    const kraken_gpio_config_t* gpio_config = board->config->gpio_config;
    // Port 0 is always SoC GPIO
    kraken_log_debug("Creating GPIO port");
    kraken_gpio_port_create((kraken_gpio_port_t**) &ports[0], gpio_config);
    // Create any auxillary MUX ports attached via I2C or SPI
    for(size_t i = 0; i < config->mux_count; i++) {// IOn
        kraken_log_debug("Creating MUX port %zu", i);
        const kraken_mux_config_t* mux_config = config->mux_configs[i];
        kraken_port_t** port = &ports[i + 1];
        switch(mux_config->type) {
            case KRAKEN_MUX_TYPE_I2C: {
                kraken_i2c_mux_port_create((kraken_i2c_mux_port_t**) port, &mux_config->i2c);
                break;
            }
            case KRAKEN_MUX_TYPE_SPI: {
                kraken_spi_mux_port_create((kraken_spi_mux_port_t**) port, &mux_config->spi);
                break;
            }
        }
    }
    board->ports = ports;

    // Optionally set up flash device if specified
    kraken_flash_t* flash = nullptr;
    if(config->flash_device) {
        kraken_log_debug("Creating flash device %s", config->flash_device);
        KRAKEN_CHECK_CALL_ERR(kraken_flash_create(&flash, config->flash_device), "Could not create flash instance");
    }
    board->flash = flash;

    *handle = (kraken_board_handle_t) board;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_config(const kraken_board_c_handle_t handle,
                                                     const kraken_board_config_t** config) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Config address pointer is null");
    const kraken_board_t* board = (const kraken_board_t*) handle;
    *config = board->config;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_flash(const kraken_board_c_handle_t handle,
                                                    kraken_flash_handle_t* flash) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    KRAKEN_CHECK_PTR(flash, KRAKEN_ERR_INVALID_ARG, "Flash handle pointer is null");
    const kraken_board_t* board = (const kraken_board_t*) handle;
    *flash = (kraken_flash_handle_t) board->flash;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_ports(const kraken_board_c_handle_t handle, kraken_port_handle_t* ports,
                                                    size_t* count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    const kraken_board_t* board = (kraken_board_t*) handle;
    if(ports) {
        memcpy(ports, board->ports, sizeof(kraken_port_handle_t) * board->num_ports);
    }
    if(count) {
        *count = board->num_ports;
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_OP, "Invalid board handle");
    kraken_board_t* board = (kraken_board_t*) handle;
    for(size_t i = 0; i < board->num_ports; i++) {
        kraken_port_t* port = board->ports[i];
        switch(port->type) {
            case KRAKEN_PORT_TYPE_GPIO: {
                kraken_log_debug("Destroying GPIO port");
                KRAKEN_CHECK_CALL_ERR(kraken_gpio_port_destroy(&port->gpio), "Could not destroy GPIO port");
                break;
            }
            default: break;
            case KRAKEN_PORT_TYPE_I2C_MUX: {
                kraken_log_debug("Destroying MUX port %zu", i);
                KRAKEN_CHECK_CALL_ERR(kraken_i2c_mux_port_destroy(&port->i2c_mux), "Could not destroy I2C MUX port");
                break;
            }
            case KRAKEN_PORT_TYPE_SPI_MUX: {
                kraken_log_debug("Destroying MUX port %zu", i);
                KRAKEN_CHECK_CALL_ERR(kraken_spi_mux_port_destroy(&port->spi_mux), "Could not destroy SPI MUX port");
                break;
            }
        }
    }
    if(board->flash) {
        kraken_log_debug("Destroying flash device");
        kraken_flash_destroy(board->flash);
    }
    kraken_log_debug("Freeing ports memory");
    kraken_free(board->ports);// Free ports array memory
    kraken_board_config_destroy(board->config);
    kraken_log_debug("Freeing board instance memory");
    kraken_free(board);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_port_for_io(const kraken_board_c_handle_t handle,
                                                          const kraken_io_c_handle_t io, kraken_port_handle_t* port) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    KRAKEN_CHECK_PTR(io, KRAKEN_ERR_INVALID_ARG, "Invalid IO handle");
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Port pointer is null");
    const kraken_io_t* io_impl = (const kraken_io_t*) io;
    const kraken_board_t* board = (const kraken_board_t*) handle;
    for(size_t port_index = 0; port_index < board->num_ports; port_index++) {
        const kraken_port_t* current_port = board->ports[port_index];
        for(size_t io_index = 0; io_index < current_port->num_ios; io_index++) {
            const kraken_io_t* current_io = current_port->ios[io_index];
            if(current_io != io_impl) {
                continue;
            }
            *port = (kraken_port_handle_t) current_port;
            return KRAKEN_OK;
        }
    }
    kraken_last_error_set("No known port associated with the given IO");
    return KRAKEN_ERR_INVALID_ARG;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_ports_for_type(const kraken_board_c_handle_t handle,
                                                             kraken_port_type_t type, kraken_port_handle_t* ports,
                                                             size_t* count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    const kraken_board_t* board = (const kraken_board_t*) handle;
    if(ports) {
        size_t matching_count = 0;
        if(count) {// Custom buffer bounds specified
            const size_t max_count = *count;
            for(size_t i = 0; i < board->num_ports; i++) {
                if(matching_count == max_count) {
                    break;
                }
                const kraken_port_t* current_port = board->ports[i];
                if(current_port->type != type) {
                    continue;
                }
                ports[matching_count++] = (kraken_port_handle_t) current_port;
            }
        }
        else {
            // We copy all ports with the given type
            for(size_t i = 0; i < board->num_ports; i++) {
                const kraken_port_t* current_port = board->ports[i];
                if(current_port->type != type) {
                    continue;
                }
                ports[matching_count++] = (kraken_port_handle_t) current_port;
            }
        }
        return KRAKEN_OK;
    }
    if(count) {
        size_t matching_count = 0;
        for(size_t i = 0; i < board->num_ports; i++) {
            const kraken_port_t* current_port = board->ports[i];
            if(current_port->type != type) {
                continue;
            }
            matching_count++;
        }
        *count = matching_count;
    }
    return KRAKEN_OK;
}