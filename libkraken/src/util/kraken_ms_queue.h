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

#ifndef LIBKRAKEN_KRAKEN_ATOMIC_QUEUE_H
#define LIBKRAKEN_KRAKEN_ATOMIC_QUEUE_H

#include "kraken_internal.h"

KRAKEN_API_BEGIN

typedef struct kraken_ms_queue_hazard_ptr {
    _Atomic(void*) ptr;
} kraken_ms_queue_hazard_ptr_t;

typedef struct kraken_ms_queue_node {
    void* value;
    _Atomic(struct kraken_ms_queue_node*) next;
} kraken_ms_queue_node_t;

typedef struct kraken_ms_queue_retired_node {
    void* ptr;
    struct kraken_ms_queue_retired_node* next;
} kraken_ms_queue_retired_node_t;

typedef struct kraken_ms_queue_tagged_ptr {
    kraken_ms_queue_node_t* const ptr;
    const uint64_t tag;
} __attribute__((aligned(16))) kraken_ms_queue_tagged_ptr_t;

typedef struct kraken_ms_queue {
    kraken_ms_queue_tagged_ptr_t head;
    kraken_ms_queue_tagged_ptr_t tail;
    size_t element_size;
    kraken_ms_queue_hazard_ptr_t** hazard_table;
} kraken_ms_queue_t;

kraken_error_t kraken_ms_queue_create(size_t element_size, kraken_ms_queue_t* queue);

kraken_error_t kraken_ms_queue_alloc(size_t element_size, kraken_ms_queue_t** queue);

kraken_error_t kraken_ms_queue_is_empty(const kraken_ms_queue_t* queue, bool* result);

kraken_error_t kraken_ms_queue_enqueue(kraken_ms_queue_t* queue, const void* value);

kraken_error_t kraken_ms_queue_dequeue(kraken_ms_queue_t* queue, void* value);

kraken_error_t kraken_ms_queue_free(kraken_ms_queue_t* queue);

kraken_error_t kraken_ms_queue_destroy(kraken_ms_queue_t* queue);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ATOMIC_QUEUE_H