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

#define kraken_heapcopy_of(p) (typeof_unqual(*p)*) kraken_heapcopy((const void*) (p), sizeof(*(p)))

KRAKEN_API_BEGIN

/// @brief Function pointer for memory allocation.
/// @param[in] size The size of the memory to allocate.
/// @return A pointer to the allocated memory, or nullptr on failure.
typedef void* (*pfn_kraken_alloc)(size_t size);

/// @brief Function pointer for memory re-allocation.
/// @param[in] memory The memory to re-allocate.
/// @param[in] size The new size of the memory.
/// @return A pointer to the re-allocated memory, or nullptr on failure.
typedef void* (*pfn_kraken_realloc)(void* memory, size_t size);

/// @brief Function pointer for memory deallocation.
/// @param[in] memory The memory to free.
typedef void (*pfn_kraken_free)(void* memory);

/// @brief A structure representing a memory allocator.
typedef struct kraken_allocator {
    pfn_kraken_alloc pfn_alloc;    ///< The allocation function.
    pfn_kraken_realloc pfn_realloc;///< The re-allocation function.
    pfn_kraken_free pfn_free;      ///< The deallocation function.
} kraken_allocator_t;

/// @brief Set the global memory allocator.
/// @param[in] allocator The allocator to set.
/// @return KRAKEN_OK on success, KRAKEN_ERR_INVALID_ARG if allocator is nullptr.
KRAKEN_EXPORT kraken_error_t kraken_set_allocator(const kraken_allocator_t* allocator);

/// @brief Get the global memory allocator.
/// @param[out] allocator A pointer to a pointer to receive the current allocator.
/// @return KRAKEN_OK on success, KRAKEN_ERR_INVALID_ARG if allocator is nullptr.
KRAKEN_EXPORT kraken_error_t kraken_get_allocator(const kraken_allocator_t** allocator);

/// @brief Allocate memory using the global allocator.
/// @param[in] size The size of the memory to allocate.
/// @return A pointer to the allocated memory, or nullptr on failure.
KRAKEN_EXPORT void* kraken_malloc(size_t size);

/// @brief Allocate and zero-initialize memory using the global allocator.
/// @param[in] size The size of the memory to allocate.
/// @return A pointer to the allocated memory, or nullptr on failure.
KRAKEN_EXPORT void* kraken_calloc(size_t size);

/// @brief Re-allocates memory using the global allocator.
/// @param[in] memory The memory to re-allocate.
/// @param[in] new_size The new size of the memory.
/// @return A pointer to the re-allocated memory, or nullptr on failure.
KRAKEN_EXPORT void* kraken_realloc(void* memory, size_t new_size);

/// @brief Duplicate a string using the global allocator.
/// @param[in] string The string to duplicate.
/// @return A pointer to the duplicated string, or nullptr on failure.
KRAKEN_EXPORT char* kraken_strdup(const char* string);

/// @brief Copy memory to a new heap allocation.
/// @param[in] memory The memory to copy.
/// @param[in] size The size of the memory to copy.
/// @return A pointer to the copied memory, or nullptr on failure.
KRAKEN_EXPORT void* kraken_heapcopy(const void* memory, size_t size);

/// @brief Free memory using the global allocator.
/// @param[in] memory The memory to free.
KRAKEN_EXPORT void kraken_free(void* memory);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ALLOC_H
