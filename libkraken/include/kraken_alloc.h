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

//
// Created by fux on 30.03.26.
//

#ifndef LIBKRAKEN_KRAKEN_ALLOC_H
#define LIBKRAKEN_KRAKEN_ALLOC_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

typedef void* (*pfn_kraken_alloc)(size_t size);
typedef void (*pfn_kraken_free)(void* memory);

typedef struct kraken_allocator {
    pfn_kraken_alloc pfn_alloc;
    pfn_kraken_free pfn_free;
} kraken_allocator_t;

KRAKEN_EXPORT kraken_error_t kraken_set_allocator(const kraken_allocator_t* allocator);
KRAKEN_EXPORT kraken_error_t kraken_get_allocator(const kraken_allocator_t** allocator);
KRAKEN_EXPORT void* kraken_malloc(size_t size);
KRAKEN_EXPORT void* kraken_calloc(size_t size);
KRAKEN_EXPORT char* kraken_strdup(const char* string);
KRAKEN_EXPORT void kraken_free(void* memory);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ALLOC_H
