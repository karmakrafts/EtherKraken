#ifndef LIBKRAKEN_KRAKEN_ERROR_H
#define LIBKRAKEN_KRAKEN_ERROR_H

#include "kraken_handles.h"

KRAKEN_API_BEGIN

typedef enum kraken_error : int32_t {
    KRAKEN_OK,
    KRAKEN_EOF,
    KRAKEN_ERR_INVALID_ARG,
    KRAKEN_ERR_INVALID_OP
} kraken_error_t;

KRAKEN_EXPORT const char* kraken_last_error_get();
KRAKEN_EXPORT void kraken_last_error_clear();

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ERROR_H
