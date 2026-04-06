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

#ifndef LIBKRAKEN_KRAKEN_ARRAY_LIST_H
#define LIBKRAKEN_KRAKEN_ARRAY_LIST_H

#include "kraken_error.h"

KRAKEN_API_BEGIN

typedef struct kraken_array_list {
    _Atomic(void*) memory;
    _Atomic(size_t) size;
    _Atomic(size_t) capacity;
    _Atomic(size_t) element_size;
} kraken_array_list_t;

kraken_error_t kraken_array_list_alloc(size_t initial_capacity, size_t element_size, kraken_array_list_t** list);

kraken_error_t kraken_array_list_create(size_t initial_capacity, size_t element_size, kraken_array_list_t* list);

kraken_error_t kraken_array_list_add(kraken_array_list_t* list, const void* element);

kraken_error_t kraken_array_list_remove(kraken_array_list_t* list, const void* element);

kraken_error_t kraken_array_list_get(kraken_array_list_t* list, size_t index, void* element);

kraken_error_t kraken_array_list_index_of(kraken_array_list_t* list, const void* element, size_t* index);

kraken_error_t kraken_array_list_set(kraken_array_list_t* list, size_t index, const void* element);

kraken_error_t kraken_array_list_contains(kraken_array_list_t* list, const void* element, bool* contains);

kraken_error_t kraken_array_list_clear(kraken_array_list_t* list);

kraken_error_t kraken_array_list_destroy(kraken_array_list_t* list);

kraken_error_t kraken_array_list_free(kraken_array_list_t* list);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ARRAY_LIST_H