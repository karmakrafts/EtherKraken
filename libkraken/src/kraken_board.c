#include "libkraken.h"

#include "bcm2835.h"
#include "kraken_board_impl.h"
#include "kraken_error.h"
#include "kraken_error_impl.h"
#include "kraken_internal.h"

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

// Compile time board configuration, change as needed
constexpr size_t NUM_GPIO_PORTS = 1;
constexpr size_t NUM_MUX_PORTS = 2;
constexpr size_t NUM_PORTS = NUM_GPIO_PORTS + NUM_MUX_PORTS;
constexpr size_t GPIOMEM_SIZE = 4096;
constexpr uint8_t MUX_START_ADDRESS = 0x20U;

static const char* g_gpio_device = "/dev/gpiomem";
static const char* g_flash_device = "/dev/mtd0";
static const char* g_mux_busses[NUM_MUX_PORTS] = {"/dev/i2c-1", "/dev/i2c-1"};

kraken_port_type_t port_type_from_mux_port_type(kraken_mux_port_type_t type) {
    switch(type) {
        case 0: return KRAKEN_PORT_IO0;
        case 1: return KRAKEN_PORT_IO1;
        default: kraken_panic("Unknown port type");
    }
}

kraken_error_t create_gpio_port(kraken_port_t** port_addr) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    kraken_port_t* port = malloc(sizeof(kraken_port_t));
    kraken_gpio_port_t* gpio_port = &port->gpio;

    const int fd = open(g_gpio_device, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open IO device at /dev/gpiomem");
    KRAKEN_CHECK_RESULT(flock(fd, LOCK_EX), KRAKEN_ERR_INVALID_OP, "Could not acquire exclusive lock on IO device");
    gpio_port->fd = fd;

    // We map the entire page but only expose the subregion where the GPIO registers are mapped into
    void* base_address = mmap(nullptr, GPIOMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    KRAKEN_CHECK_PTR(base_address, KRAKEN_ERR_INVALID_OP, "Could not map GPIO memory");
    gpio_port->registers = base_address;
    gpio_port->registers_size = sizeof(bcm2835_gpio_t);

    gpio_port->type = KRAKEN_PORT_EXT;
    *port_addr = port;
    return KRAKEN_OK;
}

kraken_error_t destroy_gpio_port(kraken_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type == KRAKEN_PORT_EXT, KRAKEN_ERR_INVALID_ARG, "Port is not a GPIO port");
    const kraken_gpio_port_t* gpio_port = &port->gpio;
    KRAKEN_CHECK_RESULT(munmap(gpio_port->registers, gpio_port->registers_size), KRAKEN_ERR_INVALID_OP,
                        "Could not unmap GPIO memory");
    KRAKEN_CHECK_RESULT(flock(gpio_port->fd, LOCK_UN), KRAKEN_ERR_INVALID_OP,
                        "Could not release exclusive lock on IO device");
    KRAKEN_CHECK_RESULT(close(gpio_port->fd), KRAKEN_ERR_INVALID_OP, "Could not close IO device");
    free(port);
    return KRAKEN_OK;
}

kraken_error_t create_mux_port(kraken_port_t** port_addr, kraken_mux_port_type_t type, uint8_t address,
                               const char* bus_device) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(address >= MUX_START_ADDRESS, KRAKEN_ERR_INVALID_ARG, "Invalid MUX address");

    kraken_port_t* port = malloc(sizeof(kraken_port_t));
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_OP, "Could not allocate MUX port");
    KRAKEN_CHECK(type < NUM_MUX_PORTS, KRAKEN_ERR_INVALID_ARG, "Invalid MUX port type");

    kraken_mux_port_t* mux_port = &port->mux;
    mux_port->type = port_type_from_mux_port_type(type);
    mux_port->index = type;
    mux_port->address = address;

    const int fd = open(bus_device, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open MUX device");
    KRAKEN_CHECK_RESULT(ioctl(fd, I2C_SLAVE, address), KRAKEN_ERR_INVALID_OP, "Could not set MUX slave address");
    mux_port->fd = fd;

    *port_addr = port;
    return KRAKEN_OK;
}

kraken_error_t destroy_mux_port(kraken_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type > KRAKEN_PORT_EXT, KRAKEN_ERR_INVALID_ARG, "Port is not a MUX port");
    const kraken_mux_port_t* mux_port = &port->mux;
    KRAKEN_CHECK_RESULT(close(mux_port->fd), KRAKEN_ERR_INVALID_OP, "Could not close MUX device");
    free(port);
    return KRAKEN_OK;
}

kraken_error_t create_flash(kraken_flash_t** flash_addr) {
    KRAKEN_CHECK_PTR(flash_addr, KRAKEN_ERR_INVALID_ARG, "Invalid flash address");

    kraken_flash_t* flash = malloc(sizeof(kraken_flash_t));
    KRAKEN_CHECK_PTR(flash, KRAKEN_ERR_INVALID_OP, "Could not allocate flash");
    flash->path = g_flash_device;

    const int fd = open(g_flash_device, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open flash device");
    flash->fd = fd;

    *flash_addr = flash;
    return KRAKEN_OK;
}

kraken_error_t destroy_flash(kraken_flash_t* flash) {
    KRAKEN_CHECK_RESULT(close(flash->fd), KRAKEN_ERR_INVALID_OP, "Could not close flash device");
    free(flash);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_create(kraken_board_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Board handle pointer is null");
    kraken_board_t* board = malloc(sizeof(kraken_board_t));
    KRAKEN_CHECK_PTR(board, KRAKEN_ERR_INVALID_OP, "Could not allocate board instance");
    board->num_ports = NUM_PORTS;

    kraken_port_t** ports = malloc(sizeof(kraken_port_t*) * NUM_PORTS);
    for(size_t i = 0; i < NUM_PORTS; i++) {
        ports[i] = nullptr;// Ensure no dangling pointers since this keeps state
    }
    KRAKEN_CHECK_PTR(ports, KRAKEN_ERR_INVALID_OP, "Could not allocate IO ports");
    for(size_t i = 0; i < NUM_GPIO_PORTS; i++) {
        create_gpio_port(&ports[i]);
    }
    for(size_t i = 0; i < NUM_MUX_PORTS; i++) {// IOn
        const char* bus_device = g_mux_busses[i];
        const uint8_t bus_address = MUX_START_ADDRESS + i;
        kraken_port_t** port = &ports[i + NUM_GPIO_PORTS];
        create_mux_port(port, i, bus_address, bus_device);
    }
    board->ports = ports;

    kraken_flash_t* flash = nullptr;
    KRAKEN_CHECK_ERROR(create_flash(&flash), "Could not create flash instance");
    board->flash = flash;

    *handle = (kraken_board_handle_t) board;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_flash(kraken_board_handle_t handle, kraken_flash_handle_t* flash) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    KRAKEN_CHECK_PTR(flash, KRAKEN_ERR_INVALID_ARG, "Flash handle pointer is null");
    const kraken_board_t* board = (const kraken_board_t*) handle;
    *flash = (kraken_flash_handle_t) board->flash;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_ports(kraken_board_handle_t handle, kraken_port_handle_t* ports,
                                                    size_t* count) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid board handle");
    const kraken_board_t* board = (kraken_board_t*) handle;
    if(ports) {
        memcpy(ports, board->ports, sizeof(kraken_port_t) * board->num_ports);
    }
    if(count) {
        *count = board->num_ports;
    }
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_aux_power_get(kraken_board_handle_t handle, kraken_aux_power_state_t* state) {
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_aux_power_set(kraken_board_handle_t handle, kraken_aux_power_state_t state) {
    // TODO: implement this
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle) {
    kraken_board_t* board = (kraken_board_t*) handle;
    for(size_t i = 0; i < NUM_GPIO_PORTS; i++) {
        KRAKEN_CHECK_ERROR(destroy_gpio_port(board->ports[i]), "Could not destroy GPIO port");
    }
    for(size_t i = 0; i < NUM_MUX_PORTS; i++) {
        KRAKEN_CHECK_ERROR(destroy_mux_port(board->ports[i + NUM_GPIO_PORTS]), "Could not destroy MUX port");
    }
    destroy_flash(board->flash);
    free(board->ports);// Free ports array memory
    free(board);
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_get_port_for_io(kraken_board_handle_t handle, kraken_io_handle_t io,
                                                          kraken_port_handle_t* port) {
    // TODO: implement this
    return KRAKEN_OK;
}