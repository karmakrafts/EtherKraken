#include "kraken_flash.h"

#include "kraken_error_impl.h"
#include "kraken_flash_impl.h"
#include "kraken_internal.h"

#include <mtd/mtd-user.h>
#include <sys/ioctl.h>

KRAKEN_EXPORT kraken_error_t kraken_flash_clear(kraken_flash_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    const kraken_flash_t* flash = (const kraken_flash_t*) handle;
    struct mtd_info_user mtd_info = {};
    KRAKEN_CHECK_RESULT(ioctl(flash->fd, MEMGETINFO, &mtd_info), KRAKEN_ERR_INVALID_OP,
                        "Could not retrieve MTD device information");
    struct erase_info_user erase_info = {// clang-format off
        .start = 0,
        .length = mtd_info.size
    };// clang-format on
    KRAKEN_CHECK_RESULT(ioctl(flash->fd, MEMERASE, &erase_info), KRAKEN_ERR_INVALID_OP, "Could not clear flash");
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_flash_get_size(kraken_flash_handle_t handle, size_t* size) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    KRAKEN_CHECK_PTR(size, KRAKEN_ERR_INVALID_ARG, "Size pointer is null");
    const kraken_flash_t* flash = (const kraken_flash_t*) handle;
    struct mtd_info_user info = {};
    KRAKEN_CHECK_RESULT(ioctl(flash->fd, MEMGETINFO, &info), KRAKEN_ERR_INVALID_OP,
                        "Could not retrieve MTD device information");
    *size = (size_t) info.size;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_flash_read(kraken_flash_handle_t handle, void* buffer, size_t size, size_t offset) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    KRAKEN_CHECK_PTR(buffer, KRAKEN_ERR_INVALID_ARG, "Buffer pointer is null");
    size_t max_size = 0;
    KRAKEN_CHECK_ERROR(kraken_flash_get_size(handle, &max_size), "Could not determine flash size");
    KRAKEN_CHECK(size > 0, KRAKEN_ERR_INVALID_ARG, "Flash read size must be > 0");
    KRAKEN_CHECK(size <= max_size, KRAKEN_ERR_INVALID_ARG, "Flash read size must be <= flash size");
    KRAKEN_CHECK(offset < max_size, KRAKEN_ERR_INVALID_ARG, "Flash read offset must be <= flash size");
    const kraken_flash_t* flash = (const kraken_flash_t*) handle;
    const int fd = flash->fd;
    lseek(fd, (ptrdiff_t) offset, SEEK_SET);
    switch(read(fd, buffer, size)) {
        case -1: return KRAKEN_ERR_INVALID_OP;
        case 0: return KRAKEN_EOF;
        default: return KRAKEN_OK;
    }
}

KRAKEN_EXPORT kraken_error_t kraken_flash_write(kraken_flash_handle_t handle, const void* buffer, size_t size,
                                                size_t offset) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid flash handle");
    size_t max_size = 0;
    KRAKEN_CHECK_ERROR(kraken_flash_get_size(handle, &max_size), "Could not determine flash size");
    KRAKEN_CHECK(size > 0, KRAKEN_ERR_INVALID_ARG, "Flash read size must be > 0");
    KRAKEN_CHECK(size <= max_size, KRAKEN_ERR_INVALID_ARG, "Flash read size must be <= flash size");
    KRAKEN_CHECK(offset < max_size, KRAKEN_ERR_INVALID_ARG, "Flash read offset must be <= flash size");
    const kraken_flash_t* flash = (kraken_flash_t*) handle;
    const int fd = flash->fd;
    lseek(fd, (ptrdiff_t) offset, SEEK_SET);
    switch(write(fd, buffer, size)) {
        case -1: return KRAKEN_ERR_INVALID_OP;
        case 0: return KRAKEN_EOF;
        default: return KRAKEN_OK;
    }
}