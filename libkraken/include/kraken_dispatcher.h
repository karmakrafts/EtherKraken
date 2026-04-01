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

#ifndef LIBKRAKEN_KRAKEN_DISPATCHER_HPP
#define LIBKRAKEN_KRAKEN_DISPATCHER_HPP

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_create(int32_t core_affinity, kraken_dispatcher_handle_t* handle);

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_register(kraken_dispatcher_handle_t handle, kraken_clock_handle_t clock);

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_unregister(kraken_dispatcher_handle_t handle,
                                                          kraken_clock_handle_t clock);

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_get_clocks(kraken_dispatcher_handle_t handle,
                                                          kraken_clock_handle_t* clocks, size_t* count);

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_destroy(kraken_dispatcher_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_DISPATCHER_HPP
