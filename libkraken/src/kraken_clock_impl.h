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

#ifndef LIBKRAKEN_KRAKEN_CLOCK_IMPL_H
#define LIBKRAKEN_KRAKEN_CLOCK_IMPL_H

#include "kraken_clock.h"
#include "util/kraken_array_list.h"
#include "util/kraken_mutex.h"

KRAKEN_API_BEGIN

typedef struct kraken_clock {
    double frequency;
    uint64_t period;
    _Atomic(uint64_t) next_event;
    kraken_array_list_t drivers;
    kraken_mutex_t drivers_mutex;
} kraken_clock_t;

kraken_error_t kraken_clock_tick(kraken_clock_t* clock);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_CLOCK_IMPL_H
