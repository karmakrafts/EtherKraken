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

#ifndef LIBKRAKEN_KRAKEN_BOARD_IMPL_H
#define LIBKRAKEN_KRAKEN_BOARD_IMPL_H

#include "kraken_flash_impl.h"
#include "port/kraken_port_impl.h"

typedef struct _kraken_board_t {//NOLINT
    kraken_board_config_t* config;
    kraken_port_t** ports;
    size_t num_ports;
    kraken_flash_t* flash;
} kraken_board_t;

#endif//LIBKRAKEN_KRAKEN_BOARD_IMPL_H