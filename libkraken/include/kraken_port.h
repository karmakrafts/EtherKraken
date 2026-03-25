#ifndef LIBKRAKEN_KRAKEN_IO_PORT_H
#define LIBKRAKEN_KRAKEN_IO_PORT_H

#include "kraken_handles.h"

KRAKEN_API_BEGIN

// Port types correspond to physical ports on the board
typedef enum kraken_port_type : uint8_t {
    KRAKEN_PORT_EXT,
    KRAKEN_PORT_IO0,
    KRAKEN_PORT_IO1
} kraken_port_type_t;

KRAKEN_EXPORT kraken_error_t kraken_port_get_type(kraken_port_handle_t port, kraken_port_type_t* type);
KRAKEN_EXPORT kraken_error_t kraken_port_get_ios(kraken_port_handle_t port, kraken_io_handle_t* ios, size_t* count);
KRAKEN_EXPORT kraken_error_t kraken_port_update(kraken_port_handle_t port);
KRAKEN_EXPORT kraken_error_t kraken_port_get_name(kraken_port_handle_t port, char* buffer, size_t* size);
KRAKEN_EXPORT kraken_error_t kraken_port_type_get_name(kraken_port_type_t type, const char** name);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_IO_PORT_H