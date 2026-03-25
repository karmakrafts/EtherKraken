#ifndef LIBKRAKEN_KRAKEN_BOARD_IMPL_H
#define LIBKRAKEN_KRAKEN_BOARD_IMPL_H

#include "kraken_flash_impl.h"
#include "kraken_port_impl.h"

typedef struct _kraken_board_t {//NOLINT
    kraken_port_t** ports;
    size_t num_ports;
    kraken_flash_t* flash;
} kraken_board_t;

#endif//LIBKRAKEN_KRAKEN_BOARD_IMPL_H