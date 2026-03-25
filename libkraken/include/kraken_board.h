#ifndef LIBKRAKEN_KRAKEN_BOARD_H
#define LIBKRAKEN_KRAKEN_BOARD_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

typedef struct _kraken_board_t* kraken_board_handle_t;// NOLINT

KRAKEN_EXPORT kraken_error_t kraken_board_init(kraken_board_handle_t* handle);
KRAKEN_EXPORT kraken_error_t kraken_board_deinit(kraken_board_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_BOARD_H
