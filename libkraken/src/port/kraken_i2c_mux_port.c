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

#include "kraken_i2c_mux_port.h"
#include "config/kraken_config_impl.h"
#include "kraken_error_impl.h"
#include "kraken_internal.h"
#include "kraken_port.h"

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

kraken_error_t kraken_i2c_mux_port_create(kraken_i2c_mux_port_t** port_addr, const kraken_i2c_mux_config_t* config) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Config pointer is null");

    kraken_i2c_mux_port_t* port = kraken_calloc(sizeof(kraken_i2c_mux_port_t));
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_OP, "Could not allocate MUX port");
    port->type = KRAKEN_PORT_TYPE_I2C_MUX;

    KRAKEN_CHECK_ERROR(
            kraken_mux_config_copy((const kraken_mux_config_t*) config, (kraken_mux_config_t**) &port->config),
            "Could not copy MUX config to port instance");

    const int fd = open(config->bus, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open MUX bus");
    KRAKEN_CHECK_RESULT(ioctl(fd, I2C_SLAVE, config->address), KRAKEN_ERR_INVALID_OP,
                        "Could not set MUX slave address");
    port->fd = fd;

    // Allocate shadow memory if requested
    const size_t shadow_mem_size = config->shadow_memory_size;
    if(shadow_mem_size > 0) {
        void* shadow_memory = kraken_calloc(shadow_mem_size);
        KRAKEN_CHECK_PTR(shadow_memory, KRAKEN_ERR_INVALID_OP, "Could not allocate I2C MUX shadow memory");
        port->shadow_memory = shadow_memory;
    }

    *port_addr = port;
    return KRAKEN_OK;
}

kraken_error_t kraken_i2c_mux_port_destroy(kraken_i2c_mux_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type > KRAKEN_PORT_TYPE_I2C_MUX, KRAKEN_ERR_INVALID_ARG, "Port is not a MUX port");
    KRAKEN_CHECK_RESULT(close(port->fd), KRAKEN_ERR_INVALID_OP, "Could not close MUX device");
    void* shadow_memory = port->shadow_memory;
    if(shadow_memory) {
        kraken_free(shadow_memory);
    }
    kraken_mux_config_destroy((kraken_mux_config_t*) port->config);
    kraken_free(port);
    return KRAKEN_OK;
}