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

#include "kraken_mutex.h"
#include "kraken_cpu.h"
#include "kraken_error_impl.h"

kraken_error_t kraken_mutex_create(kraken_mutex_t* mutex) {
    // Mutex doesn't require special initialization yet
    return KRAKEN_OK;
}

kraken_error_t kraken_mutex_lock(kraken_mutex_t* mutex) {
    KRAKEN_CHECK_PTR(mutex, KRAKEN_ERR_INVALID_ARG, "Invalid mutex pointer");
    bool expected = false;
    while(atomic_compare_exchange_weak(&mutex->is_locked, &expected, true)) {
        sched_yield();
    }
    return KRAKEN_OK;
}

kraken_error_t kraken_mutex_lock_no_yield(kraken_mutex_t* mutex) {
    KRAKEN_CHECK_PTR(mutex, KRAKEN_ERR_INVALID_ARG, "Invalid mutex pointer");
    bool expected = false;
    while(atomic_compare_exchange_weak(&mutex->is_locked, &expected, true)) {
        kraken_cpu_sleep_cycles(1);// Spin one additional cycle
    }
    return KRAKEN_OK;
}

kraken_error_t kraken_mutex_try_lock(kraken_mutex_t* mutex, bool* result) {
    KRAKEN_CHECK_PTR(mutex, KRAKEN_ERR_INVALID_ARG, "Invalid mutex pointer");
    KRAKEN_CHECK_PTR(result, KRAKEN_ERR_INVALID_ARG, "Invalid result pointer");
    bool expected = false;
    *result = atomic_compare_exchange_weak(&mutex->is_locked, &expected, true);
    return KRAKEN_OK;
}

kraken_error_t kraken_mutex_unlock(kraken_mutex_t* mutex) {
    KRAKEN_CHECK_PTR(mutex, KRAKEN_ERR_INVALID_ARG, "Invalid mutex pointer");
    atomic_store(&mutex->is_locked, false);
    return KRAKEN_OK;
}

kraken_error_t kraken_mutex_try_unlock(kraken_mutex_t* mutex, bool* result) {
    KRAKEN_CHECK_PTR(mutex, KRAKEN_ERR_INVALID_ARG, "Invalid mutex pointer");
    bool expected = true;
    *result = atomic_compare_exchange_weak(&mutex->is_locked, &expected, false);
    return KRAKEN_OK;
}

kraken_error_t kraken_mutex_destroy(kraken_mutex_t* mutex) {
    KRAKEN_CHECK_PTR(mutex, KRAKEN_ERR_INVALID_ARG, "Invalid mutex pointer");
    // Mutex doesn't require special de-initialization yet
    return KRAKEN_OK;
}