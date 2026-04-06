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

#ifndef LIBKRAKEN_KRAKEN_CONFIG_IMPL_H
#define LIBKRAKEN_KRAKEN_CONFIG_IMPL_H

#include "config/kraken_config.h"

KRAKEN_API_BEGIN

kraken_error_t kraken_board_config_copy(const kraken_board_config_t* config, kraken_board_config_t** new_config);
kraken_error_t kraken_gpio_config_copy(const kraken_gpio_config_t* config, kraken_gpio_config_t** new_config);
kraken_error_t kraken_mux_config_copy(const kraken_mux_config_t* config, kraken_mux_config_t** new_config);

kraken_error_t kraken_board_config_destroy(kraken_board_config_t* config);
kraken_error_t kraken_gpio_config_destroy(kraken_gpio_config_t* config);
kraken_error_t kraken_mux_config_destroy(kraken_mux_config_t* config);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_CONFIG_IMPL_H