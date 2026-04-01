// Copyright 2026 Karma Krafts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "kraken_gpio_port.h"
#include "config/kraken_config_impl.h"
#include "kraken_alloc.h"
#include "kraken_error_impl.h"
#include "kraken_internal.h"
#include "kraken_log_impl.h"
#include "kraken_string.h"

#include <sys/stat.h>

static kraken_error_t check_compatibility(const kraken_gpio_config_t* config) {
    char* dte_path = string_format("%s/compatible", config->device_tree_entry);
    kraken_log_debug("Reading GPIO device tree entry from %s", dte_path);
    const int dte_fd = open(dte_path, O_RDONLY);
    kraken_free(dte_path);

    kraken_log_debug("Reading GPIO DTE stats");
    struct stat stat = {};
    KRAKEN_CHECK_CALL_RES(fstat(dte_fd, &stat), KRAKEN_ERR_INVALID_OP, "Could not determine size of DTE property");

    char* dte_buffer = kraken_calloc(stat.st_size + 1);
    KRAKEN_CHECK_CALL(read(dte_fd, dte_buffer, stat.st_size) == stat.st_size, KRAKEN_ERR_INVALID_OP,
                      "Could not read GPIO DTE property");
    close(dte_fd);
    kraken_log_debug("Detected GPIO compatibility: %s", dte_buffer);
    // Check if device type is a substring of the DTE data to determine if hardware is supported
    const char* result = strstr(dte_buffer, config->device_type);
    kraken_free(dte_buffer);
    return result != nullptr ? KRAKEN_OK : KRAKEN_ERR_INVALID_OP;
}

kraken_error_t kraken_gpio_port_create(kraken_gpio_port_t** port_addr, const kraken_gpio_config_t* config) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Config pointer is null");
    KRAKEN_CHECK_CALL_ERR(check_compatibility(config), "GPIO hardware is incompatible");

    kraken_gpio_port_t* port = kraken_calloc(sizeof(kraken_gpio_port_t));
    port->type = KRAKEN_PORT_TYPE_GPIO;

    kraken_log_debug("Copying GPIO config to port instance");
    KRAKEN_CHECK_CALL_ERR(kraken_gpio_config_copy(config, &port->config),
                          "Could not copy GPIO config to port instance");

    // Acquire an exclusive FD on the GPIO device
    const int fd = open(port->config->device, O_RDWR | O_SYNC);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open GPIO device memory");
    kraken_log_debug("Opened GPIO device FD %d", fd);
    KRAKEN_CHECK_CALL_RES(flock(fd, LOCK_EX), KRAKEN_ERR_INVALID_OP, "Could not acquire exclusive lock on IO device");
    kraken_log_debug("Acquired exclusive lock on GPIO FD");
    port->fd = fd;

    // Map the GPIO registers accordingly and round mapped size up to next page boundary
    const size_t page_size = sysconf(_SC_PAGESIZE);
    kraken_log_debug("Mapping GPIO device with default page size of %zu bytes", page_size);
    const size_t registers_size = port->config->registers_size;
    const size_t align_mask = page_size - 1;
    const size_t mapped_size = registers_size + align_mask & ~align_mask;
    kraken_log_debug("Mapping GPIO device with %zu bytes (%zu bytes aligned)", registers_size, mapped_size);
    void* base_address = mmap(nullptr, mapped_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    KRAKEN_CHECK_PTR(base_address, KRAKEN_ERR_INVALID_OP, "Could not map GPIO memory");
    kraken_log_debug("Mapped GPIO device base at %p", base_address);
    port->registers = base_address;
    port->mapped_registers_size = mapped_size;

    // Create shadow memory with the same size as the registers
    void* shadow_memory = kraken_calloc(port->config->registers_size);
    KRAKEN_CHECK_PTR(shadow_memory, KRAKEN_ERR_INVALID_OP, "Could not allocate GPIO shadow memory");
    kraken_log_debug("Allocated %zu bytes of GPIO shadow memory at %p", port->config->registers_size, shadow_memory);
    port->shadow_memory = shadow_memory;

    // Create all associated IOs by iterating all pin configs
    kraken_io_t** ios = kraken_calloc(sizeof(kraken_io_t*) * port->config->pin_count);
    KRAKEN_CHECK_PTR(ios, KRAKEN_ERR_INVALID_OP, "Could not allocate GPIO port IO memory");
    kraken_log_debug("Allocated IO memory at %p", (void*) ios);
    constexpr kraken_io_mode_t supported_modes[2] = {KRAKEN_IO_MODE_OUT, KRAKEN_IO_MODE_IN};
    for(size_t index = 0; index < port->config->pin_count; index++) {
        const kraken_pin_config_t* pin_config = &port->config->pins[index];
        char* name = string_format("IO%u (Pin %u)", pin_config->device_pin, pin_config->port_pin);
        KRAKEN_CHECK_PTR(name, KRAKEN_ERR_INVALID_OP, "Could not format IO name");
        kraken_log_debug("Creating %s", name);
        KRAKEN_CHECK_CALL_ERR(
                kraken_io_create(&ios[index], name, pin_config, supported_modes, KRAKEN_ARRAY_SIZE(supported_modes)),
                "Could not create IO for GPIO port");
        kraken_free(name);
    }
    port->ios = ios;
    port->num_ios = port->config->pin_count;

    *port_addr = port;
    return KRAKEN_OK;
}

kraken_error_t kraken_gpio_port_destroy(kraken_gpio_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type == KRAKEN_PORT_TYPE_GPIO, KRAKEN_ERR_INVALID_ARG, "Port is not a GPIO port");
    kraken_log_debug("Unmapping GPIO device memory");
    KRAKEN_CHECK_CALL_RES(munmap(port->registers, port->mapped_registers_size), KRAKEN_ERR_INVALID_OP,
                          "Could not unmap GPIO memory");
    kraken_log_debug("Releasing exclusive lock on GPIO FD");
    KRAKEN_CHECK_CALL_RES(flock(port->fd, LOCK_UN), KRAKEN_ERR_INVALID_OP,
                          "Could not release exclusive lock on IO device");
    kraken_log_debug("Closing GPIO FD");
    KRAKEN_CHECK_CALL_RES(close(port->fd), KRAKEN_ERR_INVALID_OP, "Could not close IO device");
    for(size_t index = 0; index < port->num_ios; index++) {
        kraken_io_destroy(port->ios[index]);
    }
    kraken_log_debug("Freeing GPIO port IO memory");
    kraken_free(port->ios);
    kraken_log_debug("Freeing GPIO port shadow memory");
    kraken_free(port->shadow_memory);
    kraken_gpio_config_destroy(port->config);
    kraken_log_debug("Freeing GPIO port instance memory");
    kraken_free(port);
    return KRAKEN_OK;
}