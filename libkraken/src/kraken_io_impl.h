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

#ifndef LIBKRAKEN_KRAKEN_IO_IMPL_H
#define LIBKRAKEN_KRAKEN_IO_IMPL_H

#include "config/kraken_config.h"
#include "kraken_api.h"
#include "kraken_io.h"

typedef struct kraken_io {
    char* name;
    kraken_pin_config_t pin_config;
    kraken_io_mode_t* supported_modes;
    size_t num_supported_modes;
    kraken_io_pud_mode_t* supported_pud_modes;
    size_t num_supported_pud_modes;
    _Atomic(kraken_io_mode_t) mode;
    _Atomic(kraken_bool_t) state;
} kraken_io_t;

kraken_error_t kraken_io_create(kraken_io_t** io_addr, const char* name, const kraken_pin_config_t* pin_config,
                                const kraken_io_mode_t* supported_modes, size_t num_supported_modes);

kraken_error_t kraken_io_destroy(kraken_io_t* io);

#endif//LIBKRAKEN_KRAKEN_IO_IMPL_H