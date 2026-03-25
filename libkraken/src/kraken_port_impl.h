#ifndef LIBKRAKEN_KRAKEN_PORT_IMPL_H
#define LIBKRAKEN_KRAKEN_PORT_IMPL_H

#include "bcm2835.h"
#include "kraken_port.h"

typedef uint8_t kraken_mux_port_type_t;
typedef uint8_t kraken_mux_port_address_t;

typedef struct kraken_gpio_port {
    kraken_port_type_t type;
    int fd;
    bcm2835_gpio_t* registers;// Base address is start of GPIO registers
    size_t registers_size;
} kraken_gpio_port_t;

typedef struct kraken_mux_port {
    kraken_port_type_t type;
    int fd;                           // I2C serial transactions happen over regular read/write
    kraken_mux_port_type_t index;     // Port index (0=IO0, 1=IO1)
    kraken_mux_port_address_t address;// Bus address (starting at 0x20)
} kraken_mux_port_t;

typedef struct kraken_port {
    union {
        struct {
            kraken_port_type_t type;
            int fd;
        };
        kraken_gpio_port_t gpio;
        kraken_mux_port_t mux;
    };
} kraken_port_t;

#endif//LIBKRAKEN_KRAKEN_PORT_IMPL_H