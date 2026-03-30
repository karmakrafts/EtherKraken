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

KRAKEN_EXPORT kraken_error_t kraken_port_get_type(const kraken_port_c_handle_t handle, kraken_port_type_t* type) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Port handle is null");
    KRAKEN_CHECK_PTR(type, KRAKEN_ERR_INVALID_ARG, "Type pointer is null");
    const kraken_port_t* port = (const kraken_port_t*) handle;
    *type = port->type;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_get_ios(const kraken_port_c_handle_t handle, kraken_io_handle_t* ios,
                                                 size_t* count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Port handle is null");
    const kraken_port_t* port = (const kraken_port_t*) handle;
    if(ios) {
        memcpy(ios, port->ios, sizeof(kraken_io_handle_t) * port->num_ios);
    }
    if(count) {
        *count = port->num_ios;
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_update(const kraken_port_c_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Port handle is null");
    const kraken_port_t* port = (const kraken_port_t*) handle;
    switch(port->type) {
        case KRAKEN_PORT_TYPE_GPIO: {
            const kraken_gpio_port_t* gpio_port = &port->gpio;
            KRAKEN_CHECK_PTR(gpio_port->config->pfn_state_update, KRAKEN_ERR_INVALID_OP,
                             "GPIO port is missing state update callback");
            // clang-format off
            gpio_port->config->pfn_state_update(
                gpio_port->registers,
                gpio_port->shadow_memory,
                (kraken_io_handle_t*)gpio_port->ios,
                gpio_port->num_ios);
            // clang-format on
            break;
        }
        case KRAKEN_PORT_TYPE_I2C_MUX: {
            const kraken_i2c_mux_port_t* mux_port = &port->i2c_mux;
            KRAKEN_CHECK_PTR(mux_port->config->pfn_state_update, KRAKEN_ERR_INVALID_OP,
                             "I2C MUX port is missing state update callback");
            // clang-format off
            mux_port->config->pfn_state_update(
                mux_port->fd,
                mux_port->shadow_memory,
                (kraken_io_handle_t*)mux_port->ios,
                mux_port->num_ios);
            // clang-format on
            break;
        }
        case KRAKEN_PORT_TYPE_SPI_MUX: {
            const kraken_spi_mux_port_t* mux_port = &port->spi_mux;
            KRAKEN_CHECK_PTR(mux_port->config->pfn_state_update, KRAKEN_ERR_INVALID_OP,
                             "SPI MUX port is missing state update callback");
            // clang-format off
            mux_port->config->pfn_state_update(
                mux_port->registers,
                mux_port->shadow_memory,
                (kraken_io_handle_t*)mux_port->ios,
                mux_port->num_ios);
            // clang-format on
            break;
        }
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_reinit(const kraken_port_c_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Port handle is null");
    const kraken_port_t* port = (const kraken_port_t*) handle;
    switch(port->type) {
        case KRAKEN_PORT_TYPE_GPIO: {
            const kraken_gpio_port_t* gpio_port = &port->gpio;
            KRAKEN_CHECK_PTR(gpio_port->config->pfn_state_init, KRAKEN_ERR_INVALID_OP,
                             "GPIO port is missing state initialization callback");
            // clang-format off
            gpio_port->config->pfn_state_init(
                gpio_port->registers,
                gpio_port->shadow_memory,
                (kraken_io_handle_t*)gpio_port->ios,
                gpio_port->num_ios);
            // clang-format on
            break;
        }
        case KRAKEN_PORT_TYPE_I2C_MUX: {
            const kraken_i2c_mux_port_t* mux_port = &port->i2c_mux;
            KRAKEN_CHECK_PTR(mux_port->config->pfn_state_init, KRAKEN_ERR_INVALID_OP,
                             "I2C MUX port is missing state initialization callback");
            // clang-format off
            mux_port->config->pfn_state_init(
                mux_port->fd,
                mux_port->shadow_memory,
                (kraken_io_handle_t*)mux_port->ios,
                mux_port->num_ios);
            // clang-format on
            break;
        }
        case KRAKEN_PORT_TYPE_SPI_MUX: {
            const kraken_spi_mux_port_t* mux_port = &port->spi_mux;
            KRAKEN_CHECK_PTR(mux_port->config->pfn_state_init, KRAKEN_ERR_INVALID_OP,
                             "SPI MUX port is missing state initialization callback");
            // clang-format off
            mux_port->config->pfn_state_init(
                mux_port->registers,
                mux_port->shadow_memory,
                (kraken_io_handle_t*)mux_port->ios,
                mux_port->num_ios);
            // clang-format on
            break;
        }
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_get_name(const kraken_port_c_handle_t handle, const char** name) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid port handle");
    KRAKEN_CHECK_PTR(name, KRAKEN_ERR_INVALID_ARG, "Invalid name address pointer");
    const kraken_port_t* port = (const kraken_port_t*) handle;
    switch(port->type) {
        case KRAKEN_PORT_TYPE_GPIO: {
            *name = port->gpio.config->alias;
            break;
        }
        case KRAKEN_PORT_TYPE_I2C_MUX: {
            *name = port->i2c_mux.config->alias;
            break;
        }
        case KRAKEN_PORT_TYPE_SPI_MUX: {
            *name = port->spi_mux.config->alias;
            break;
        }
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_port_type_get_name(const kraken_port_type_t type, const char** name) {
    KRAKEN_CHECK_PTR(name, KRAKEN_ERR_INVALID_ARG, "Name pointer is null");
    switch(type) {
        case KRAKEN_PORT_TYPE_GPIO: {
            *name = "GPIO";
            break;
        }
        case KRAKEN_PORT_TYPE_I2C_MUX: {
            *name = "I2C MUX";
            break;
        }
        case KRAKEN_PORT_TYPE_SPI_MUX: {
            *name = "SPI MUX";
            break;
        }
        default: {
            *name = "Unknown";
            break;
        }
    }
    return KRAKEN_OK;
}