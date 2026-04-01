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
#include "kraken_error_impl.h"

#include <malloc.h>

static kraken_allocator_t g_default_allocator = {// clang-format off
    .pfn_alloc = &malloc,
    .pfn_realloc = &realloc,
    .pfn_free = &free
};// clang-format on

static _Atomic(const kraken_allocator_t*) g_allocator = &g_default_allocator;

KRAKEN_EXPORT kraken_error_t kraken_set_allocator(const kraken_allocator_t* allocator) {
    KRAKEN_CHECK_PTR(allocator, KRAKEN_ERR_INVALID_ARG, "Invalid allocator pointer");
    g_allocator = allocator;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_get_allocator(const kraken_allocator_t** allocator) {
    KRAKEN_CHECK_PTR(allocator, KRAKEN_ERR_INVALID_ARG, "Invalid allocator address pointer");
    *allocator = g_allocator;
    return KRAKEN_OK;
}

KRAKEN_EXPORT void* kraken_malloc(const size_t size) {
    return g_allocator->pfn_alloc(size);
}

KRAKEN_EXPORT void* kraken_calloc(const size_t size) {
    void* memory = g_allocator->pfn_alloc(size);
    if(memory == nullptr) {
        return nullptr;
    }
    memset(memory, 0x00, size);
    return memory;
}

KRAKEN_EXPORT void* kraken_realloc(void* memory, const size_t new_size) {
    if(memory == nullptr) {
        return nullptr;
    }
    return g_allocator->pfn_realloc(memory, new_size);
}

KRAKEN_EXPORT char* kraken_strdup(const char* string) {
    if(string == nullptr) {
        return nullptr;
    }
    const size_t size = strlen(string) + 1;
    char* memory = kraken_malloc(size);
    if(memory == nullptr) {
        return nullptr;
    }
    memcpy(memory, string, size);
    return memory;
}

KRAKEN_EXPORT void kraken_free(void* memory) {
    g_allocator->pfn_free(memory);
}