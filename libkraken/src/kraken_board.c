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
#include "kraken_error.h"
#include "kraken_error_impl.h"
#include "kraken_internal.h"

#include <linux/i2c-dev.h>
#include <mtd/mtd-user.h>
#include <sys/ioctl.h>

static kraken_error_t create_gpio_port(kraken_port_t** port_addr, const kraken_gpio_config_t* config) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Config pointer is null");
    kraken_port_t* port = malloc(sizeof(kraken_port_t));
    kraken_gpio_port_t* gpio_port = &port->gpio;
    gpio_port->type = KRAKEN_PORT_TYPE_GPIO;

    memcpy(&gpio_port->config, config, sizeof(kraken_gpio_config_t));

    const int fd = open(config->device, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open IO device at /dev/gpiomem");
    KRAKEN_CHECK_RESULT(flock(fd, LOCK_EX), KRAKEN_ERR_INVALID_OP, "Could not acquire exclusive lock on IO device");
    gpio_port->fd = fd;

    KRAKEN_CHECK(config->registers_size <= config->mapped_size, KRAKEN_ERR_INVALID_ARG,
                 "Register region must be <= mapped size");
    // We map the entire page but only expose the subregion where the GPIO registers are mapped into
    void* base_address = mmap(nullptr, config->mapped_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    KRAKEN_CHECK_PTR(base_address, KRAKEN_ERR_INVALID_OP, "Could not map GPIO memory");
    gpio_port->registers = base_address;
    gpio_port->registers_size = config->registers_size;

    *port_addr = port;
    return KRAKEN_OK;
}

static kraken_error_t destroy_gpio_port(kraken_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type == KRAKEN_PORT_TYPE_GPIO, KRAKEN_ERR_INVALID_ARG, "Port is not a GPIO port");
    const kraken_gpio_port_t* gpio_port = &port->gpio;
    KRAKEN_CHECK_RESULT(munmap(gpio_port->registers, gpio_port->registers_size), KRAKEN_ERR_INVALID_OP,
                        "Could not unmap GPIO memory");
    KRAKEN_CHECK_RESULT(flock(gpio_port->fd, LOCK_UN), KRAKEN_ERR_INVALID_OP,
                        "Could not release exclusive lock on IO device");
    KRAKEN_CHECK_RESULT(close(gpio_port->fd), KRAKEN_ERR_INVALID_OP, "Could not close IO device");
    free(port);
    return KRAKEN_OK;
}

static kraken_error_t create_mux_port(kraken_port_t** port_addr, const uint32_t index,
                                      const kraken_mux_config_t* config) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Config pointer is null");

    kraken_port_t* port = malloc(sizeof(kraken_port_t));
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_OP, "Could not allocate MUX port");

    kraken_mux_port_t* mux_port = &port->mux;
    mux_port->type = KRAKEN_PORT_TYPE_MUX;
    mux_port->index = index;
    mux_port->address = config->address;

    const int fd = open(config->bus, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open MUX bus");
    KRAKEN_CHECK_RESULT(ioctl(fd, I2C_SLAVE, config->address), KRAKEN_ERR_INVALID_OP,
                        "Could not set MUX slave address");
    mux_port->fd = fd;

    *port_addr = port;
    return KRAKEN_OK;
}

static kraken_error_t destroy_mux_port(kraken_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type > KRAKEN_PORT_TYPE_MUX, KRAKEN_ERR_INVALID_ARG, "Port is not a MUX port");
    const kraken_mux_port_t* mux_port = &port->mux;
    KRAKEN_CHECK_RESULT(close(mux_port->fd), KRAKEN_ERR_INVALID_OP, "Could not close MUX device");
    free(port);
    return KRAKEN_OK;
}

static kraken_error_t create_flash(kraken_flash_t** flash_addr, const char* device) {
    KRAKEN_CHECK_PTR(flash_addr, KRAKEN_ERR_INVALID_ARG, "Invalid flash address");

    kraken_flash_t* flash = malloc(sizeof(kraken_flash_t));
    KRAKEN_CHECK_PTR(flash, KRAKEN_ERR_INVALID_OP, "Could not allocate flash");
    flash->path = device;

    const int fd = open(device, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open flash device");
    flash->fd = fd;

    struct mtd_info_user mtd_info = {};
    KRAKEN_CHECK_RESULT(ioctl(flash->fd, MEMGETINFO, &mtd_info), KRAKEN_ERR_INVALID_OP,
                        "Could not retrieve MTD device information");
    flash->size = (size_t) mtd_info.size;

    *flash_addr = flash;
    return KRAKEN_OK;
}

static kraken_error_t destroy_flash(kraken_flash_t* flash) {
    KRAKEN_CHECK_RESULT(close(flash->fd), KRAKEN_ERR_INVALID_OP, "Could not close flash device");
    free(flash);
    return KRAKEN_OK;
}

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
    create_gpio_port(&ports[0], gpio_config);
    for(size_t i = 0; i < config->mux_count; i++) {// IOn
        const kraken_mux_config_t* mux_config = &config->mux_configs[i];
        kraken_port_t** port = &ports[i + 1];
        create_mux_port(port, i, mux_config);
    }
    board->ports = ports;

    // Optionally set up flash device if specified
    kraken_flash_t* flash = nullptr;
    if(config->flash_device) {
        KRAKEN_CHECK_ERROR(create_flash(&flash, config->flash_device), "Could not create flash instance");
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
    KRAKEN_CHECK_ERROR(destroy_gpio_port(board->ports[0]), "Could not destroy GPIO port");
    for(size_t i = 0; i < board->config.mux_count; i++) {
        KRAKEN_CHECK_ERROR(destroy_mux_port(board->ports[i + 1]), "Could not destroy MUX port");
    }
    if(board->flash) {
        destroy_flash(board->flash);
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