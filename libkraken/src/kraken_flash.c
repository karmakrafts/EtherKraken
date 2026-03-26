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

#include "kraken_flash.h"

#include "kraken_error_impl.h"
#include "kraken_flash_impl.h"
#include "kraken_internal.h"

#include <mtd/mtd-user.h>
#include <sys/ioctl.h>

KRAKEN_EXPORT kraken_error_t kraken_flash_clear(kraken_flash_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    const kraken_flash_t* flash = (const kraken_flash_t*) handle;
    struct erase_info_user erase_info = {// clang-format off
        .start = 0,
        .length = (uint32_t)flash->size
    };// clang-format on
    KRAKEN_CHECK_RESULT(ioctl(flash->fd, MEMERASE, &erase_info), KRAKEN_ERR_INVALID_OP, "Could not clear flash");
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_flash_get_size(const kraken_flash_c_handle_t handle, size_t* size) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    KRAKEN_CHECK_PTR(size, KRAKEN_ERR_INVALID_ARG, "Size pointer is null");
    const kraken_flash_t* flash = (const kraken_flash_t*) handle;
    *size = flash->size;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_flash_read(kraken_flash_handle_t handle, void* buffer, const size_t size,
                                               const size_t offset) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    KRAKEN_CHECK_PTR(buffer, KRAKEN_ERR_INVALID_ARG, "Buffer pointer is null");
    KRAKEN_CHECK(size > 0, KRAKEN_ERR_INVALID_ARG, "Flash read size must be > 0");
    const kraken_flash_t* flash = (const kraken_flash_t*) handle;
    KRAKEN_CHECK(size <= flash->size, KRAKEN_ERR_INVALID_ARG, "Flash read size must be <= flash size");
    KRAKEN_CHECK(offset < flash->size, KRAKEN_ERR_INVALID_ARG, "Flash read offset must be <= flash size");
    const int fd = flash->fd;
    lseek(fd, (ptrdiff_t) offset, SEEK_SET);
    switch(read(fd, buffer, size)) {
        case -1: return KRAKEN_ERR_INVALID_OP;
        case 0: return KRAKEN_EOF;
        default: return KRAKEN_OK;
    }
}

KRAKEN_EXPORT kraken_error_t kraken_flash_write(kraken_flash_handle_t handle, const void* buffer, const size_t size,
                                                const size_t offset) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    KRAKEN_CHECK(size > 0, KRAKEN_ERR_INVALID_ARG, "Flash read size must be > 0");
    const kraken_flash_t* flash = (kraken_flash_t*) handle;
    KRAKEN_CHECK(size <= flash->size, KRAKEN_ERR_INVALID_ARG, "Flash read size must be <= flash size");
    KRAKEN_CHECK(offset < flash->size, KRAKEN_ERR_INVALID_ARG, "Flash read offset must be <= flash size");
    const int fd = flash->fd;
    lseek(fd, (ptrdiff_t) offset, SEEK_SET);
    switch(write(fd, buffer, size)) {
        case -1: return KRAKEN_ERR_INVALID_OP;
        case 0: return KRAKEN_EOF;
        default: return KRAKEN_OK;
    }
}