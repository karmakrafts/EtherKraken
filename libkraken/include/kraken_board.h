#ifndef LIBKRAKEN_KRAKEN_BOARD_H
#define LIBKRAKEN_KRAKEN_BOARD_H

#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

// On-state of the on-board 3.3V supply rails to allow efficient deep-sleep
typedef enum kraken_aux_power_state : uint8_t {
    KRAKEN_AUX_POWER_STATE_ON,
    KRAKEN_AUX_POWER_STATE_OFF
} kraken_aux_power_state_t;

KRAKEN_EXPORT kraken_error_t kraken_board_create(kraken_board_handle_t* handle);
KRAKEN_EXPORT kraken_error_t kraken_board_get_ports(kraken_board_handle_t handle, kraken_port_handle_t* ports,
                                                    size_t* count);
KRAKEN_EXPORT kraken_error_t kraken_board_get_flash(kraken_board_handle_t handle, kraken_flash_handle_t* flash);
KRAKEN_EXPORT kraken_error_t kraken_board_aux_power_get(kraken_board_handle_t handle, kraken_aux_power_state_t* state);
KRAKEN_EXPORT kraken_error_t kraken_board_aux_power_set(kraken_board_handle_t handle, kraken_aux_power_state_t state);
KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle);
KRAKEN_EXPORT kraken_error_t kraken_board_get_port_for_io(kraken_board_handle_t handle, kraken_io_handle_t io,
                                                          kraken_port_handle_t* port);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_BOARD_H
