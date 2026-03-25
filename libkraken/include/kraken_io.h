#ifndef LIBKRAKEN_KRAKEN_PIN_H
#define LIBKRAKEN_KRAKEN_PIN_H

#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

typedef enum kraken_io_mode : uint8_t {
    KRAKEN_IO_MODE_IN,
    KRAKEN_IO_MODE_OUT
} kraken_io_mode_t;

KRAKEN_EXPORT kraken_error_t kraken_io_get_supported_modes(kraken_io_handle_t io, kraken_io_mode_t* modes,
                                                           size_t* count);
KRAKEN_EXPORT kraken_error_t kraken_io_get_mode(kraken_io_handle_t io, kraken_io_mode_t* mode);
KRAKEN_EXPORT kraken_error_t kraken_io_set_mode(kraken_io_handle_t io, kraken_io_mode_t mode);
KRAKEN_EXPORT kraken_error_t kraken_io_get(kraken_io_handle_t io, uint8_t* state);
KRAKEN_EXPORT kraken_error_t kraken_io_set(kraken_io_handle_t io, uint8_t state);
KRAKEN_EXPORT kraken_error_t kraken_io_get_name(kraken_io_handle_t io, char* buffer, size_t* size);
KRAKEN_EXPORT kraken_error_t kraken_io_get_pin_number(kraken_io_handle_t io, size_t* pin_number);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_PIN_H
