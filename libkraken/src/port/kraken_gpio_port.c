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
#include "kraken_error_impl.h"
#include "kraken_internal.h"

#include <sys/stat.h>

static kraken_error_t check_compatibility(const kraken_gpio_config_t* config) {
    char* dte_path = nullptr;
    KRAKEN_CHECK(asprintf(&dte_path, "%s/compatible", config->device_tree_entry) > 0, KRAKEN_ERR_INVALID_OP,
                 "Could not determine GPIO DTE path");
    const int dte_fd = open(dte_path, O_RDONLY);
    free(dte_path);
    struct stat stat = {};
    KRAKEN_CHECK_RESULT(fstat(dte_fd, &stat), KRAKEN_ERR_INVALID_OP, "Could not determine size of DTE property");
    char dte_buffer[stat.st_size + 1];
    dte_buffer[stat.st_size] = '\0';
    KRAKEN_CHECK(read(dte_fd, dte_buffer, stat.st_size) == stat.st_size, KRAKEN_ERR_INVALID_OP,
                 "Could not read GPIO DTE property");
    close(dte_fd);
    // Check if device type is a substring of the DTE data to determine if hardware is supported
    return strstr(dte_buffer, config->device_type) != nullptr ? KRAKEN_OK : KRAKEN_ERR_INVALID_OP;
}

kraken_error_t kraken_gpio_port_create(kraken_gpio_port_t** port_addr, const kraken_gpio_config_t* config) {
    KRAKEN_CHECK_PTR(port_addr, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK_PTR(config, KRAKEN_ERR_INVALID_ARG, "Config pointer is null");
    KRAKEN_CHECK_ERROR(check_compatibility(config), "GPIO hardware is incompatible");

    kraken_gpio_port_t* port = kraken_alloc(kraken_gpio_port_t);
    port->type = KRAKEN_PORT_TYPE_GPIO;

    memcpy(&port->config, config, sizeof(kraken_gpio_config_t));

    const int fd = open(config->device, O_RDWR);
    KRAKEN_CHECK(fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open GPIO device memory");
    KRAKEN_CHECK_RESULT(flock(fd, LOCK_EX), KRAKEN_ERR_INVALID_OP, "Could not acquire exclusive lock on IO device");
    port->fd = fd;

    // Map the GPIO registers accordingly and round mapped size up to next page boundary
    const size_t page_size = sysconf(_SC_PAGESIZE);
    const size_t registers_size = config->registers_size;
    const size_t align_mask = page_size - 1;
    const size_t mapped_size = registers_size + align_mask & ~align_mask;
    void* base_address = mmap(nullptr, mapped_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    KRAKEN_CHECK_PTR(base_address, KRAKEN_ERR_INVALID_OP, "Could not map GPIO memory");
    port->registers = base_address;
    port->mapped_registers_size = mapped_size;

    // Create shadow memory with the same size as the registers
    void* shadow_memory = calloc(1, config->registers_size);
    KRAKEN_CHECK_PTR(shadow_memory, KRAKEN_ERR_INVALID_OP, "Could not allocate GPIO shadow memory");
    port->shadow_memory = shadow_memory;

    *port_addr = port;
    return KRAKEN_OK;
}

kraken_error_t kraken_gpio_port_destroy(kraken_gpio_port_t* port) {
    KRAKEN_CHECK_PTR(port, KRAKEN_ERR_INVALID_ARG, "Invalid port address");
    KRAKEN_CHECK(port->type == KRAKEN_PORT_TYPE_GPIO, KRAKEN_ERR_INVALID_ARG, "Port is not a GPIO port");
    KRAKEN_CHECK_RESULT(munmap(port->registers, port->mapped_registers_size), KRAKEN_ERR_INVALID_OP,
                        "Could not unmap GPIO memory");
    KRAKEN_CHECK_RESULT(flock(port->fd, LOCK_UN), KRAKEN_ERR_INVALID_OP,
                        "Could not release exclusive lock on IO device");
    KRAKEN_CHECK_RESULT(close(port->fd), KRAKEN_ERR_INVALID_OP, "Could not close IO device");
    free(port->shadow_memory);
    free(port);
    return KRAKEN_OK;
}