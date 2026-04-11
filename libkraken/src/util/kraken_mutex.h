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

#ifndef LIBKRAKEN_KRAKEN_IO_MUTEX_H
#define LIBKRAKEN_KRAKEN_IO_MUTEX_H

#include "kraken_api.h"
#include "kraken_error.h"

// A small custom mutex implementation that allows locking/spinning without yielding

KRAKEN_API_BEGIN

typedef struct kraken_mutex {
    _Atomic(bool) is_locked;
} kraken_mutex_t;

kraken_error_t kraken_mutex_create(kraken_mutex_t* mutex);

kraken_error_t kraken_mutex_lock(kraken_mutex_t* mutex);

kraken_error_t kraken_mutex_lock_no_yield(kraken_mutex_t* mutex);

kraken_error_t kraken_mutex_try_lock(kraken_mutex_t* mutex, bool* result);

kraken_error_t kraken_mutex_unlock(kraken_mutex_t* mutex);

kraken_error_t kraken_mutex_try_unlock(kraken_mutex_t* mutex, bool* result);

kraken_error_t kraken_mutex_destroy(kraken_mutex_t* mutex);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_IO_MUTEX_H