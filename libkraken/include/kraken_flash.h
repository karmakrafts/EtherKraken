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

#ifndef LIBKRAKEN_KRAKEN_FLASH_H
#define LIBKRAKEN_KRAKEN_FLASH_H

#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

/// @brief Clears the contents of the flash memory.
///
/// @param[in] handle The handle to the flash memory to clear.
/// @return KRAKEN_OK on success, or an error code on failure.
KRAKEN_EXPORT kraken_error_t kraken_flash_clear(kraken_flash_handle_t handle);

/// @brief Gets the total size of the flash memory.
///
/// @param[in] handle The handle to the flash memory to query.
/// @param[out] size Pointer to a variable that will receive the size of the flash memory in bytes.
/// @return KRAKEN_OK on success, or an error code on failure.
KRAKEN_EXPORT kraken_error_t kraken_flash_get_size(kraken_flash_c_handle_t handle, size_t* size);

/// @brief Reads data from the flash memory.
///
/// @param[in] handle The handle to the flash memory to read from.
/// @param[out] buffer Pointer to the buffer that will receive the read data.
/// @param[in] size The number of bytes to read.
/// @param[in] offset The byte offset in the flash memory to start reading from.
/// @return KRAKEN_OK on success, or an error code on failure.
KRAKEN_EXPORT kraken_error_t kraken_flash_read(kraken_flash_handle_t handle, void* buffer, size_t size, size_t offset);

/// @brief Writes data to the flash memory.
///
/// @param[in] handle The handle to the flash memory to write to.
/// @param[in] buffer Pointer to the buffer containing the data to write.
/// @param[in] size The number of bytes to write.
/// @param[in] offset The byte offset in the flash memory to start writing to.
/// @return KRAKEN_OK on success, or an error code on failure.
KRAKEN_EXPORT kraken_error_t kraken_flash_write(kraken_flash_handle_t handle, const void* buffer, size_t size,
                                                size_t offset);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_FLASH_H