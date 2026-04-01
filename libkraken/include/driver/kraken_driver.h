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

#ifndef LIBKRAKEN_KRAKEN_DRIVER_H
#define LIBKRAKEN_KRAKEN_DRIVER_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

typedef void (*pfn_kraken_driver_tick)(kraken_port_handle_t port, void* user_data);

KRAKEN_EXPORT kraken_error_t kraken_driver_create(kraken_port_handle_t port, pfn_kraken_driver_tick callback,
                                                  void* user_data, kraken_driver_handle_t* handle);

KRAKEN_EXPORT kraken_error_t kraken_driver_destroy(kraken_driver_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_DRIVER_H