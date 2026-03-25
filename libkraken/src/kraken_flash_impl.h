#ifndef LIBKRAKEN_KRAKEN_FLASH_IMPL_H
#define LIBKRAKEN_KRAKEN_FLASH_IMPL_H

typedef struct kraken_flash {
    const char* path;
    int fd;
} kraken_flash_t;

#endif//LIBKRAKEN_KRAKEN_FLASH_IMPL_H