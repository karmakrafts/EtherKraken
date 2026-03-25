#ifndef LIBKRAKEN_KRAKEN_FLASH_H
#define LIBKRAKEN_KRAKEN_FLASH_H

#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

KRAKEN_EXPORT kraken_error_t kraken_flash_clear(kraken_flash_handle_t handle);
KRAKEN_EXPORT kraken_error_t kraken_flash_get_size(kraken_flash_handle_t handle, size_t* size);
KRAKEN_EXPORT kraken_error_t kraken_flash_read(kraken_flash_handle_t handle, void* buffer, size_t size, size_t offset);
KRAKEN_EXPORT kraken_error_t kraken_flash_write(kraken_flash_handle_t handle, const void* buffer, size_t size,
                                                size_t offset);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_FLASH_H