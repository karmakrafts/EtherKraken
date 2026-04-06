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

#include "kraken_ms_queue.h"
#include "kraken_alloc.h"
#include "kraken_cpu.h"

constexpr int32_t HP_PER_THREAD = 2;
constexpr int32_t RETIRE_THRESHOLD = 16;

// Hazard pointers

static uint32_t g_hp_max_threads = 0;
static _Atomic(int32_t) g_hp_thread_count = 0;
static thread_local int32_t g_hp_thread_id = -1;
static once_flag g_hp_init_flag = ONCE_FLAG_INIT;

static void byte_queue_hp_init_global() {
    long int count = sysconf(_SC_NPROCESSORS_ONLN);
    if(count <= 0) {
        count = 4;
    }
    g_hp_max_threads = count << 1;// Assume two threads per core
}

static void byte_queue_hp_init(kraken_ms_queue_t* queue) {
    call_once(&g_hp_init_flag, &byte_queue_hp_init_global);
    queue->hazard_table = kraken_calloc(sizeof(kraken_ms_queue_hazard_ptr_t*) * g_hp_max_threads);
    if(!queue->hazard_table) {
        kraken_panic("Could not allocate queue hazard pointer table");
    }
    for(uint32_t thread_index = 0; thread_index < g_hp_max_threads; thread_index++) {
        kraken_ms_queue_hazard_ptr_t* ptrs = kraken_calloc(sizeof(kraken_ms_queue_hazard_ptr_t) * HP_PER_THREAD);
        if(!ptrs) {
            kraken_panic("Could not allocate hazard pointers for thread %u", thread_index);
        }
        queue->hazard_table[thread_index] = ptrs;
    }
}

static void byte_queue_hp_cleanup(const kraken_ms_queue_t* queue) {
    for(uint32_t thread_index = 0; thread_index < g_hp_max_threads; thread_index++) {
        kraken_free(queue->hazard_table[thread_index]);
    }
    kraken_free(queue->hazard_table);
}

static int32_t byte_queue_hp_register_thread() {
    if(g_hp_thread_id != -1) {
        return g_hp_thread_id;
    }
    call_once(&g_hp_init_flag, &byte_queue_hp_init_global);
    const int32_t id = atomic_fetch_add(&g_hp_thread_count, 1);
    if(id >= g_hp_max_threads) {
        kraken_panic("Could not register thread for queue, too many threads");
    }
    g_hp_thread_id = id;
    return id;
}

static bool byte_queue_hp_is_hazard(const kraken_ms_queue_t* queue, const void* ptr) {
    const int32_t threads = atomic_load(&g_hp_thread_count);
    for(int32_t thread_index = 0; thread_index < threads; thread_index++) {
        for(int32_t hp_index = 0; hp_index < HP_PER_THREAD; hp_index++) {
            if(atomic_load(&queue->hazard_table[thread_index][hp_index].ptr) == ptr) {
                return true;
            }
        }
    }
    return false;
}

static void byte_queue_hp_set(const kraken_ms_queue_t* queue, const int32_t slot, void* ptr) {
    atomic_store(&queue->hazard_table[g_hp_thread_id][slot].ptr, ptr);
}

static void byte_queue_hp_clear(const kraken_ms_queue_t* queue, const int32_t slot) {
    atomic_store(&queue->hazard_table[g_hp_thread_id][slot].ptr, nullptr);
}

// Retired node list

static thread_local kraken_ms_queue_retired_node_t* g_retired_list = nullptr;
static thread_local uint64_t g_retired_count = 0;

static void byte_queue_rl_scan(const kraken_ms_queue_t* queue) {
    kraken_ms_queue_retired_node_t** current = &g_retired_list;
    while(*current) {
        if(!byte_queue_hp_is_hazard(queue, (*current)->ptr)) {
            kraken_ms_queue_retired_node_t* temp = *current;
            *current = temp->next;
            kraken_ms_queue_node_t* node = temp->ptr;
            kraken_free(node->value);
            kraken_free(node);
            kraken_free(temp);
            g_retired_count--;
            continue;
        }
        current = &(*current)->next;
    }
}

static void byte_queue_rl_retire(const kraken_ms_queue_t* queue, void* ptr) {
    kraken_ms_queue_retired_node_t* node = kraken_malloc(sizeof(kraken_ms_queue_retired_node_t));
    if(!node) {
        return;
    }
    node->ptr = ptr;
    node->next = g_retired_list;
    g_retired_list = node;
    if(++g_retired_count >= RETIRE_THRESHOLD) {
        byte_queue_rl_scan(queue);
    }
}

// Tagged pointer atomic operations

static KRAKEN_INLINE kraken_ms_queue_tagged_ptr_t byte_queue_tagged_ptr_load(const kraken_ms_queue_tagged_ptr_t* ptr) {
    const kraken_ms_queue_node_t* const node_ptr = __atomic_load_n(&ptr->ptr, __ATOMIC_RELAXED);
    const uint64_t tag = __atomic_load_n(&ptr->tag, __ATOMIC_RELAXED);
    const kraken_ms_queue_tagged_ptr_t result = {.ptr = (kraken_ms_queue_node_t*) node_ptr, .tag = tag};
    return result;
}

static KRAKEN_INLINE void byte_queue_tagged_ptr_store(kraken_ms_queue_tagged_ptr_t* ptr,
                                                      const kraken_ms_queue_tagged_ptr_t value) {
    __atomic_store_n((kraken_ms_queue_node_t**) &ptr->ptr, value.ptr, __ATOMIC_RELAXED);
    __atomic_store_n((uint64_t*) &ptr->tag, value.tag, __ATOMIC_RELAXED);
}

static KRAKEN_INLINE bool byte_queue_tagged_ptr_cas(kraken_ms_queue_tagged_ptr_t* ptr,
                                                    kraken_ms_queue_tagged_ptr_t* expected,
                                                    const kraken_ms_queue_tagged_ptr_t desired) {
    return __atomic_compare_exchange(ptr, expected, (kraken_ms_queue_tagged_ptr_t*) &desired, true, __ATOMIC_SEQ_CST,
                                     __ATOMIC_SEQ_CST);
}

// API

kraken_error_t kraken_ms_queue_create(const size_t element_size, kraken_ms_queue_t* queue) {
    KRAKEN_CHECK_PTR(queue, KRAKEN_ERR_INVALID_ARG, "Invalid queue pointer");
    KRAKEN_CHECK(element_size > 0, KRAKEN_ERR_INVALID_ARG, "Invalid queue element size");
    queue->element_size = element_size;
    byte_queue_hp_init(queue);// Initialize hazard table

    kraken_ms_queue_node_t* dummy = kraken_malloc(sizeof(kraken_ms_queue_node_t));
    KRAKEN_CHECK_PTR(dummy, KRAKEN_ERR_INVALID_OP, "Could not allocate dummy node");
    dummy->value = nullptr;
    atomic_store(&dummy->next, nullptr);

    const kraken_ms_queue_tagged_ptr_t initial_ptr = {.ptr = dummy, .tag = 0};
    byte_queue_tagged_ptr_store(&queue->head, initial_ptr);
    byte_queue_tagged_ptr_store(&queue->tail, initial_ptr);

    return KRAKEN_OK;
}

kraken_error_t kraken_ms_queue_alloc(const size_t element_size, kraken_ms_queue_t** queue) {
    kraken_ms_queue_t* new_queue = kraken_calloc(sizeof(kraken_ms_queue_t));
    KRAKEN_CHECK_PTR(new_queue, KRAKEN_ERR_INVALID_OP, "Could not allocate new queue");
    KRAKEN_CHECK_CALL_ERR(kraken_ms_queue_create(element_size, new_queue), "Could not initialize new queue");
    *queue = new_queue;
    return KRAKEN_OK;
}

kraken_error_t kraken_ms_queue_is_empty(const kraken_ms_queue_t* queue, bool* result) {
    KRAKEN_CHECK_PTR(queue, KRAKEN_ERR_INVALID_ARG, "Invalid queue pointer");
    KRAKEN_CHECK_PTR(result, KRAKEN_ERR_INVALID_ARG, "Invalid result pointer");
    byte_queue_hp_register_thread();
    while(true) {
        const kraken_ms_queue_tagged_ptr_t head = byte_queue_tagged_ptr_load(&queue->head);
        const kraken_ms_queue_node_t* head_node = head.ptr;
        byte_queue_hp_set(queue, 0, (void*) head_node);
        const kraken_ms_queue_tagged_ptr_t head_after_hp_set = byte_queue_tagged_ptr_load(&queue->head);
        if(head.ptr != head_after_hp_set.ptr || head.tag != head_after_hp_set.tag) {
            continue;
        }
        const kraken_ms_queue_node_t* next = atomic_load(&head_node->next);
        *result = next == nullptr;
        byte_queue_hp_clear(queue, 0);
        return KRAKEN_OK;
    }
}

kraken_error_t kraken_ms_queue_enqueue(kraken_ms_queue_t* queue, const void* value) {
    KRAKEN_CHECK_PTR(queue, KRAKEN_ERR_INVALID_ARG, "Invalid queue pointer");
    byte_queue_hp_register_thread();

    kraken_ms_queue_node_t* new_node = kraken_malloc(sizeof(kraken_ms_queue_node_t));
    KRAKEN_CHECK_PTR(new_node, KRAKEN_ERR_INVALID_OP, "Could not allocate queue node");

    void* owned_value = kraken_malloc(queue->element_size);
    if(!owned_value) {
        kraken_free(new_node);
        KRAKEN_CHECK_PTR(nullptr, KRAKEN_ERR_INVALID_OP, "Could not allocate queue node value");
    }
    memcpy(owned_value, value, queue->element_size);
    new_node->value = owned_value;

    atomic_store(&new_node->next, nullptr);

    while(true) {
        kraken_ms_queue_tagged_ptr_t tail = byte_queue_tagged_ptr_load(&queue->tail);
        kraken_ms_queue_node_t* tail_ptr = tail.ptr;
        byte_queue_hp_set(queue, 0, tail_ptr);
        const kraken_ms_queue_tagged_ptr_t tail_after_hp_set = byte_queue_tagged_ptr_load(&queue->tail);
        if(tail.ptr != tail_after_hp_set.ptr) {
            continue;
        }
        kraken_ms_queue_node_t* next = atomic_load(&tail_ptr->next);
        const kraken_ms_queue_tagged_ptr_t current_tail = byte_queue_tagged_ptr_load(&queue->tail);
        if(tail.ptr == current_tail.ptr && tail.tag == current_tail.tag) {
            if(!next) {
                if(atomic_compare_exchange_weak(&tail_ptr->next, &next, new_node)) {
                    const kraken_ms_queue_tagged_ptr_t new_tail = {// clang-format off
                        .ptr = new_node,
                        .tag = tail.tag + 1
                    };// clang-format on
                    byte_queue_tagged_ptr_cas(&queue->tail, &tail, new_tail);
                    byte_queue_hp_clear(queue, 0);
                    return KRAKEN_OK;
                }
            }
            else {
                const kraken_ms_queue_tagged_ptr_t new_tail = {// clang-format off
                    .ptr = next,
                    .tag = tail.tag + 1
                };// clang-format on
                byte_queue_tagged_ptr_cas(&queue->tail, &tail, new_tail);
            }
        }
    }
}

kraken_error_t kraken_ms_queue_dequeue(kraken_ms_queue_t* queue, void* value) {
    KRAKEN_CHECK_PTR(queue, KRAKEN_ERR_INVALID_ARG, "Invalid queue pointer");
    KRAKEN_CHECK_PTR(value, KRAKEN_ERR_INVALID_ARG, "Invalid value pointer");
    byte_queue_hp_register_thread();
    while(true) {
        kraken_ms_queue_tagged_ptr_t head = byte_queue_tagged_ptr_load(&queue->head);
        kraken_ms_queue_tagged_ptr_t tail = byte_queue_tagged_ptr_load(&queue->tail);
        kraken_ms_queue_node_t* head_ptr = head.ptr;
        byte_queue_hp_set(queue, 0, head_ptr);
        const kraken_ms_queue_tagged_ptr_t head_after_hp_set_0 = byte_queue_tagged_ptr_load(&queue->head);
        if(head.ptr != head_after_hp_set_0.ptr) {
            continue;
        }
        kraken_ms_queue_node_t* next = atomic_load(&head_ptr->next);
        byte_queue_hp_set(queue, 1, next);
        const kraken_ms_queue_tagged_ptr_t head_after_hp_set_1 = byte_queue_tagged_ptr_load(&queue->head);
        if(head.ptr != head_after_hp_set_1.ptr) {
            continue;
        }
        if(!next) {
            byte_queue_hp_clear(queue, 0);
            byte_queue_hp_clear(queue, 1);
            return KRAKEN_ERR_INVALID_OP;
        }
        if(head.ptr == tail.ptr) {
            const kraken_ms_queue_tagged_ptr_t new_tail = {// clang-format off
                .ptr = next,
                .tag = tail.tag + 1
            };// clang-format on
            byte_queue_tagged_ptr_cas(&queue->tail, &tail, new_tail);
            continue;
        }
        const void* next_value = next->value;
        const kraken_ms_queue_tagged_ptr_t new_head = {// clang-format off
            .ptr = next,
            .tag = head.tag + 1
        };// clang-format on
        if(byte_queue_tagged_ptr_cas(&queue->head, &head, new_head)) {
            memcpy(value, next_value, queue->element_size);
            byte_queue_hp_clear(queue, 0);
            byte_queue_hp_clear(queue, 1);
            byte_queue_rl_retire(queue, head_ptr);
            return KRAKEN_OK;
        }
    }
}

kraken_error_t kraken_ms_queue_free(kraken_ms_queue_t* queue) {
    KRAKEN_CHECK_PTR(queue, KRAKEN_ERR_INVALID_ARG, "Invalid queue pointer");
    kraken_ms_queue_destroy(queue);
    kraken_free(queue);
    return KRAKEN_OK;
}

kraken_error_t kraken_ms_queue_destroy(kraken_ms_queue_t* queue) {
    KRAKEN_CHECK_PTR(queue, KRAKEN_ERR_INVALID_ARG, "Invalid queue pointer");
    void* value = kraken_malloc(queue->element_size);
    if(value) {
        while(kraken_ms_queue_dequeue(queue, value) == KRAKEN_OK) {
            kraken_cpu_sleep_cycles(1);
        }
        kraken_free(value);
    }
    const kraken_ms_queue_tagged_ptr_t head = byte_queue_tagged_ptr_load(&queue->head);
    kraken_ms_queue_node_t* head_ptr = head.ptr;
    if(head_ptr) {
        kraken_free(head_ptr->value);
        kraken_free(head_ptr);
    }
    byte_queue_hp_cleanup(queue);// Cleanup internal hazard table
    return KRAKEN_OK;
}