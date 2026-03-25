#include "libkraken.h"

#include "kraken_board_impl.h"
#include "kraken_error_impl.h"
#include "kraken_internal.h"

constexpr size_t GPIOMEM_SIZE = 4096;// GPIO device specifies using a single page

KRAKEN_EXPORT kraken_error_t kraken_board_create(kraken_board_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Board handle pointer is null");
    kraken_board_t* board = malloc(sizeof(kraken_board_t));
    // Obtain a file descriptor to the GPIO userspace block device
    const int mem_fd = open("/dev/gpiomem", O_RDWR);
    KRAKEN_CHECK(mem_fd != -1, KRAKEN_ERR_INVALID_OP, "Could not open GPIO file descriptor");
    board->gpio_mem_fd = mem_fd;
    flock(mem_fd, LOCK_EX);// Acquire exclusive lock on file
    // Map the GPIO device into memory for read- and write-access
    void* gpio_mem = mmap(nullptr, GPIOMEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
    KRAKEN_CHECK_PTR(gpio_mem, KRAKEN_ERR_INVALID_OP, "Could not map GPIO memory");
    board->gpio_mem = gpio_mem;
    *handle = (kraken_board_handle_t) board;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_board_destroy(kraken_board_handle_t handle) {
    kraken_board_t* board = (kraken_board_t*) handle;
    KRAKEN_CHECK_RESULT(munmap(board->gpio_mem, GPIOMEM_SIZE), KRAKEN_ERR_INVALID_OP, "Could not unmap GPIO memory");
    KRAKEN_CHECK_RESULT(flock(board->gpio_mem_fd, LOCK_UN), KRAKEN_ERR_INVALID_OP, "Could not release GPIO file lock");
    KRAKEN_CHECK_RESULT(close(board->gpio_mem_fd), KRAKEN_ERR_INVALID_OP, "Could not close GPIO file descriptor");
    free(board);
    return KRAKEN_OK;
}