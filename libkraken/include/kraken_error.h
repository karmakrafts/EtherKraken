#ifndef LIBKRAKEN_KRAKEN_ERROR_H
#define LIBKRAKEN_KRAKEN_ERROR_H

#include "kraken_api.h"
#include <stdint.h>

KRAKEN_API_BEGIN

typedef int32_t kraken_error_t;

constexpr kraken_error_t KRAKEN_OK = 0;
constexpr kraken_error_t KRAKEN_ERR_INVALID_ARG = 1;
constexpr kraken_error_t KRAKEN_ERR_INVALID_OP = 2;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ERROR_H
