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
#include "kraken_board_impl.h"
#include "kraken_error_impl.h"
#include "kraken_flash_impl.h"
#include "kraken_internal.h"
#include "port/kraken_gpio_port.h"

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
    kraken_board_t* board = malloc(sizeof(kraken_board_t));
    KRAKEN_CHECK_PTR(board, KRAKEN_ERR_INVALID_OP, "Could not allocate board instance");

    memcpy(&board->config, config, sizeof(kraken_board_config_t));// Copy config to board instance
    const size_t num_ports = config->mux_count + 1;
    board->num_ports = num_ports;

    kraken_port_t** ports = malloc(sizeof(kraken_port_t*) * num_ports);
    for(size_t i = 0; i < num_ports; i++) {
        ports[i] = nullptr;// Ensure no dangling pointers since this keeps state
    }
    KRAKEN_CHECK_PTR(ports, KRAKEN_ERR_INVALID_OP, "Could not allocate IO ports");
    const kraken_gpio_config_t* gpio_config = &config->gpio_config;
    // Port 0 is always SoC GPIO
    kraken_gpio_port_create((kraken_gpio_port_t**) &ports[0], gpio_config);
    // Create any auxillary MUX ports attached via I2C or SPI
    for(size_t i = 0; i < config->mux_count; i++) {// IOn
        const kraken_mux_config_t* mux_config = &config->mux_configs[i];
        kraken_port_t** port = &ports[i + 1];
        switch(mux_config->type) {
            case KRAKEN_MUX_TYPE_I2C: {
                kraken_i2c_mux_port_create((kraken_i2c_mux_port_t**) port, &mux_config->i2c);
                break;
            }
            case KRAKEN_MUX_TYPE_SPI: {
                // TODO: implement this
                break;
            }
        }
    }
    board->ports = ports;

    // Optionally set up flash device if specified
    kraken_flash_t* flash = nullptr;
    if(config->flash_device) {
        KRAKEN_CHECK_ERROR(kraken_flash_create(&flash, config->flash_device), "Could not create flash instance");
    }
    board->flash = flash;

    *handle = (kraken_board_handle_t) board;
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
        memcpy(ports, board->ports, sizeof(kraken_port_t) * board->num_ports);
    }
    if(count) {
        *count = board->num_ports;
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_aux_power_get(const kraken_board_c_handle_t handle,
                                                        kraken_aux_power_state_t* state) {
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_aux_power_set(kraken_board_handle_t handle, kraken_aux_power_state_t state) {
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle) {
    kraken_board_t* board = (kraken_board_t*) handle;
    KRAKEN_CHECK_ERROR(kraken_gpio_port_destroy((kraken_gpio_port_t*) board->ports[0]), "Could not destroy GPIO port");
    // TODO: re-implement this
    //for(size_t i = 0; i < board->config.mux_count; i++) {
    //    KRAKEN_CHECK_ERROR(destroy_mux_port(board->ports[i + 1]), "Could not destroy MUX port");
    //}
    if(board->flash) {
        kraken_flash_destroy(board->flash);
    }
    free(board->ports);// Free ports array memory
    free(board);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_port_for_io(const kraken_board_c_handle_t handle,
                                                          const kraken_io_c_handle_t io, kraken_port_handle_t* port) {
    // TODO: implement this
    return KRAKEN_OK;
}