#ifndef LIBKRAKEN_KRAKEN_BOARD_H
#define LIBKRAKEN_KRAKEN_BOARD_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

typedef struct kraken_board_t* kraken_board_handle_t;

/// Initializing a board instance will acquire exclusive read-write access of /dev/gpiomem
/// to communicate with the SiPs GPIO blocks without requiring syscalls for every operation.
///
/// @param handle The handle to the board pointer to populate after initialization.
/// @return KRAKEN_OK if successful, KRAKEN_ERR_INVALID_ARG when the given handle is null.
KRAKEN_EXPORT kraken_error_t kraken_board_create(kraken_board_handle_t* handle);

// TODO: document this
KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_BOARD_H
