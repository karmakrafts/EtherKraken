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

#include "config/kraken_config.h"
#include "kraken_alloc.h"
#include "kraken_config_impl.h"
#include "kraken_error_impl.h"

kraken_error_t kraken_board_config_copy(const kraken_board_config_t* config, kraken_board_config_t** new_config) {
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid board config pointer");
    KRAKEN_CHECK_PTR(new_config, KRAKEN_ERR_INVALID_ARG, "Invalid board config address pointer");

    kraken_board_config_t* copy = kraken_calloc(sizeof(kraken_board_config_t));
    KRAKEN_CHECK_PTR(copy, KRAKEN_ERR_INVALID_OP, "Could not allocate board config memory");

    if(config->flash_device) {
        copy->flash_device = kraken_strdup(config->flash_device);
        KRAKEN_CHECK_PTR(copy->flash_device, KRAKEN_ERR_INVALID_OP, "Could not copy flash device name");
    }

    kraken_gpio_config_t* gpio_config_copy = nullptr;
    KRAKEN_CHECK_CALL_ERR(kraken_gpio_config_copy(config->gpio_config, &gpio_config_copy),
                          "Could not copy GPIO config");
    copy->gpio_config = gpio_config_copy;

    const size_t mux_count = config->mux_count;
    kraken_mux_config_t** mux_configs_copy = kraken_calloc(sizeof(kraken_mux_config_t*) * mux_count);
    for(size_t index = 0; index < mux_count; index++) {
        const kraken_mux_config_t* mux_config = config->mux_configs[index];
        kraken_mux_config_t* mux_config_copy = nullptr;
        KRAKEN_CHECK_CALL_ERR(kraken_mux_config_copy(mux_config, &mux_config_copy), "Could not copy MUX config");
        mux_configs_copy[index] = mux_config_copy;
    }
    copy->mux_configs = mux_configs_copy;
    copy->mux_count = mux_count;

    *new_config = copy;
    return KRAKEN_OK;
}

kraken_error_t kraken_gpio_config_copy(const kraken_gpio_config_t* config, kraken_gpio_config_t** new_config) {
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid GPIO config pointer");
    KRAKEN_CHECK_PTR(new_config, KRAKEN_ERR_INVALID_ARG, "Invalid GPIO config address pointer");

    kraken_gpio_config_t* copy = kraken_calloc(sizeof(kraken_gpio_config_t));
    KRAKEN_CHECK_PTR(copy, KRAKEN_ERR_INVALID_ARG, "Could not allocate GPIO config memory");

    copy->device_tree_entry = kraken_strdup(config->device_tree_entry);
    KRAKEN_CHECK_PTR(copy->device_tree_entry, KRAKEN_ERR_INVALID_OP, "Could not copy GPIO DTE");

    copy->device_type = kraken_strdup(config->device_type);
    KRAKEN_CHECK_PTR(copy->device_type, KRAKEN_ERR_INVALID_OP, "Could not copy GPIO device type");

    copy->device = kraken_strdup(config->device);
    KRAKEN_CHECK_PTR(copy->device, KRAKEN_ERR_INVALID_OP, "Could not copy GPIO device path");

    copy->alias = kraken_strdup(config->alias);
    KRAKEN_CHECK_PTR(copy->alias, KRAKEN_ERR_INVALID_OP, "Could not copy GPIO port alias");

    kraken_pin_config_t* pins_copy = kraken_calloc(sizeof(kraken_pin_config_t) * config->pin_count);
    KRAKEN_CHECK_PTR(pins_copy, KRAKEN_ERR_INVALID_OP, "Could not allocate GPIO pin config memory");
    for(size_t index = 0; index < config->pin_count; index++) {
        const kraken_pin_config_t* pin_config = &config->pins[index];
        kraken_pin_config_t* pin_config_copy = &pins_copy[index];
        memcpy(pin_config_copy, pin_config, sizeof(kraken_pin_config_t));
    }
    copy->pins = pins_copy;
    copy->pin_count = config->pin_count;

    copy->registers_size = config->registers_size;
    copy->pfn_state_update = config->pfn_state_update;
    copy->pfn_state_init = config->pfn_state_init;

    *new_config = copy;
    return KRAKEN_OK;
}

kraken_error_t kraken_mux_config_copy(const kraken_mux_config_t* config, kraken_mux_config_t** new_config) {
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid MUX config pointer");
    KRAKEN_CHECK_PTR(new_config, KRAKEN_ERR_INVALID_ARG, "Invalid MUX config address pointer");

    kraken_mux_config_t* copy = kraken_calloc(sizeof(kraken_mux_config_t));
    KRAKEN_CHECK_PTR(copy, KRAKEN_ERR_INVALID_ARG, "Could not allocate MUX config memory");

    // TODO: implement this

    *new_config = copy;
    return KRAKEN_OK;
}

kraken_error_t kraken_board_config_destroy(kraken_board_config_t* config) {
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid board config pointer");
    if(config->flash_device) {
        kraken_free(config->flash_device);
    }
    kraken_gpio_config_destroy(config->gpio_config);
    for(size_t index = 0; index < config->mux_count; index++) {
        kraken_mux_config_destroy(config->mux_configs[index]);
    }
    kraken_free(config->mux_configs);
    kraken_free(config);
    return KRAKEN_OK;
}

kraken_error_t kraken_gpio_config_destroy(kraken_gpio_config_t* config) {
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid GPIO config pointer");
    kraken_free(config->device_tree_entry);
    kraken_free(config->device_type);
    kraken_free(config->device);
    kraken_free(config->alias);
    kraken_free(config->pins);
    kraken_free(config);
    return KRAKEN_OK;
}

kraken_error_t kraken_mux_config_destroy(kraken_mux_config_t* config) {
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Invalid MUX config pointer");
    switch(config->type) {
        case KRAKEN_MUX_TYPE_I2C: {
            const kraken_i2c_mux_config_t* i2c_config = &config->i2c;
            kraken_free(i2c_config->alias);
            kraken_free(i2c_config->pins);
            kraken_free(i2c_config->bus);
            break;
        }
        case KRAKEN_MUX_TYPE_SPI: {
            const kraken_spi_mux_config_t* spi_config = &config->spi;
            kraken_free(spi_config->alias);
            kraken_free(spi_config->pins);
            kraken_free(spi_config->device);
            break;
        }
    }
    kraken_free(config);
    return KRAKEN_OK;
}