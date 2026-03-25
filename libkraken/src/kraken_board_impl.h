#ifndef LIBKRAKEN_KRAKEN_BOARD_IMPL_H
#define LIBKRAKEN_KRAKEN_BOARD_IMPL_H

typedef struct _kraken_board_t {
    int gpio_mem_fd;    // The FD of /dev/gpiomem for the current board instance
    void* gpio_mem;     // The base address of the GPIO MMIO block
} kraken_board_t;

#endif //LIBKRAKEN_KRAKEN_BOARD_IMPL_H