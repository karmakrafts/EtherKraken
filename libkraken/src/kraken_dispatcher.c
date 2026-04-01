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

#include "kraken_alloc.h"
#include "kraken_clock_impl.h"
#include "kraken_cpu.h"
#include "kraken_dispatcher_impl.h"
#include "kraken_error_impl.h"

#include <pthread.h>
#include <sched.h>

#undef sched_priority

static void* kraken_dispatcher_main(void* user_data) {
    kraken_dispatcher_t* dispatcher = user_data;
    uint64_t now = 0;
    while(dispatcher->is_running) {
        const size_t size = dispatcher->clocks.size;
        if(size == 0) {
            kraken_cpu_sleep_cycles(1);// Spin for one additional cycle to wait for clocks
            continue;
        }
        kraken_cpu_get_counter(&now);
        uint64_t next_event = UINT64_MAX;
        pthread_mutex_lock(&dispatcher->clocks_mutex);
        for(size_t index = 0; index < size; index++) {
            kraken_clock_t* clock = nullptr;
            if(kraken_array_list_get(&dispatcher->clocks, index, &clock) != KRAKEN_OK) {
                continue;
            }
            const size_t period = clock->period;
            if(clock->next_event <= now) {
                kraken_clock_tick(clock);
                clock->next_event += period;
                if(clock->next_event <= now) {
                    const size_t missed = (now - clock->next_event) / period + 1;
                    clock->next_event += missed * period;
                }
            }
            if(clock->next_event < next_event) {
                next_event = clock->next_event;
            }
        }
        pthread_mutex_unlock(&dispatcher->clocks_mutex);
        if(next_event > now) {
            kraken_cpu_sleep_cycles(next_event - now);
        }
    }
    return nullptr;
}

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_create(const int32_t core_affinity, kraken_dispatcher_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid dispatcher address pointer");
    kraken_dispatcher_t* dispatcher = kraken_calloc(sizeof(kraken_dispatcher_t));

    KRAKEN_CHECK_CALL_RES(pthread_mutex_init(&dispatcher->clocks_mutex, nullptr), KRAKEN_ERR_INVALID_OP,
                          "Could not initialize dispatcher clocks mutex");

    dispatcher->is_running = true;
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_create(4, sizeof(kraken_clock_handle_t), &dispatcher->clocks),
                          "Could not allocate dispatcher clock list");

    const struct sched_param sched_param = {.sched_priority = 80};
    pthread_attr_t attr = {};
    KRAKEN_CHECK_CALL_RES(pthread_attr_init(&attr), KRAKEN_ERR_INVALID_OP,
                          "Could not initialize dispatcher thread attributes");
    KRAKEN_CHECK_CALL_RES(pthread_attr_setschedpolicy(&attr, SCHED_FIFO), KRAKEN_ERR_INVALID_OP,
                          "Could not set dispatcher scheduler policy");
    KRAKEN_CHECK_CALL_RES(pthread_attr_setschedparam(&attr, &sched_param), KRAKEN_ERR_INVALID_OP,
                          "Could not set dispatcher scheduler priority");
    KRAKEN_CHECK_CALL_RES(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED), KRAKEN_ERR_INVALID_OP,
                          "Could not set explicit scheduling mode for dispatcher");

    pthread_t thread = -1;
    KRAKEN_CHECK_CALL_RES(pthread_create(&thread, &attr, &kraken_dispatcher_main, dispatcher), KRAKEN_ERR_INVALID_OP,
                          "Could not create dispatcher thread");
    KRAKEN_CHECK(thread != -1, KRAKEN_ERR_INVALID_OP, "Could not create dispatcher thread");
    KRAKEN_CHECK_CALL_RES(pthread_attr_destroy(&attr), KRAKEN_ERR_INVALID_OP,
                          "Could not destroy dispatcher thread attributes");
    dispatcher->thread = thread;
    // Assign core affinity
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(core_affinity, &cpu_set);
    KRAKEN_CHECK_CALL_RES(pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpu_set), KRAKEN_ERR_INVALID_OP,
                          "Could not assign dispatcher thread affinity");

    *handle = (kraken_dispatcher_handle_t) dispatcher;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_register(kraken_dispatcher_handle_t handle,
                                                        kraken_clock_handle_t clock) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid dispatcher handle");
    KRAKEN_CHECK_PTR(clock, KRAKEN_ERR_INVALID_ARG, "Invalid clock handle");
    kraken_dispatcher_t* dispatcher = (kraken_dispatcher_t*) handle;
    pthread_mutex_lock(&dispatcher->clocks_mutex);
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_add(&dispatcher->clocks, &clock), "Could not add clock to dispatcher list");
    pthread_mutex_unlock(&dispatcher->clocks_mutex);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_unregister(kraken_dispatcher_handle_t handle,
                                                          kraken_clock_handle_t clock) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid dispatcher handle");
    KRAKEN_CHECK_PTR(clock, KRAKEN_ERR_INVALID_ARG, "Invalid clock handle");
    kraken_dispatcher_t* dispatcher = (kraken_dispatcher_t*) handle;
    pthread_mutex_lock(&dispatcher->clocks_mutex);
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_remove(&dispatcher->clocks, &clock),
                          "Could not remove clock from dispatcher list");
    pthread_mutex_unlock(&dispatcher->clocks_mutex);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_get_clocks(kraken_dispatcher_handle_t handle,
                                                          kraken_clock_handle_t* clocks, size_t* count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid dispatcher handle");
    KRAKEN_CHECK_PTR(clocks, KRAKEN_ERR_INVALID_ARG, "Invalid clock handles pointer");
    KRAKEN_CHECK_PTR(count, KRAKEN_ERR_INVALID_ARG, "Invalid count pointer");
    kraken_dispatcher_t* dispatcher = (kraken_dispatcher_t*) handle;
    if(clocks) {
        pthread_mutex_lock(&dispatcher->clocks_mutex);
        if(count) {
            memcpy(clocks, dispatcher->clocks.memory, sizeof(kraken_clock_handle_t) * *count);
            pthread_mutex_unlock(&dispatcher->clocks_mutex);
            return KRAKEN_OK;
        }
        memcpy(clocks, dispatcher->clocks.memory, sizeof(kraken_clock_handle_t) * dispatcher->clocks.size);
        pthread_mutex_unlock(&dispatcher->clocks_mutex);
        return KRAKEN_OK;
    }
    if(count) {
        *count = dispatcher->clocks.size;
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_dispatcher_destroy(kraken_dispatcher_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid dispatcher handle");
    kraken_dispatcher_t* dispatcher = (kraken_dispatcher_t*) handle;
    dispatcher->is_running = false;
    void* return_value = nullptr;
    KRAKEN_CHECK_CALL_RES(pthread_join(dispatcher->thread, &return_value), KRAKEN_ERR_INVALID_OP,
                          "Could not join dispatcher thread");
    KRAKEN_CHECK_CALL_ERR(kraken_array_list_destroy(&dispatcher->clocks), "Could not destroy dispatcher clock list");
    KRAKEN_CHECK_CALL_RES(pthread_mutex_destroy(&dispatcher->clocks_mutex), KRAKEN_ERR_INVALID_OP,
                          "Could not destroy dispatcher clocks mutex");
    free(dispatcher);
    return KRAKEN_OK;
}