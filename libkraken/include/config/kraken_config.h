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

#ifndef LIBKRAKEN_KRAKEN_CONFIG_H
#define LIBKRAKEN_KRAKEN_CONFIG_H

#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

/// @brief Pin configuration for a device.
typedef struct kraken_pin_config {
    uint32_t device_pin;///< Pin on the actual IC.
    uint32_t port_pin;  ///< Pin on the port header on the board.
} kraken_pin_config_t;

/// @brief Callback function type for updating GPIO state.
/// @param[in] base_address The base address of the GPIO device.
/// @param[in,out] shadow_memory A pointer to the shadow memory for the device.
/// @param[in] ios A pointer to an array of IO handles.
/// @param[in] io_count The number of IO handles in the array.
/// @return KRAKEN_OK on success, or an error code on failure.
typedef kraken_error_t (*pfn_kraken_gpio_state_update)(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                       size_t io_count, uint64_t mask);

/// @brief Callback function type for initializing GPIO state.
/// @param[in] base_address The base address of the GPIO device.
/// @param[out] shadow_memory A pointer to the shadow memory for the device.
/// @param[in] ios A pointer to an array of IO handles.
/// @param[in] io_count The number of IO handles in the array.
/// @return KRAKEN_OK on success, or an error code on failure.
typedef kraken_error_t (*pfn_kraken_gpio_state_init)(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                     size_t io_count, uint64_t mask);

/// @brief Configuration for a GPIO device.
typedef struct kraken_gpio_config {
    char* device_tree_entry;                      ///< Path to the device tree entry.
    char* device_type;                            ///< Type of the device (e.g., "bcm2835").
    char* device;                                 ///< Path to the device file.
    char* alias;                                  ///< Alias for the device (e.g., "EXT", "IO0").
    size_t registers_size;                        ///< Size of the register region in bytes.
    kraken_pin_config_t* pins;                    ///< Pointer to pin configurations.
    size_t pin_count;                             ///< Number of pins.
    pfn_kraken_gpio_state_update pfn_state_update;///< State update function.
    pfn_kraken_gpio_state_init pfn_state_init;    ///< State initialization function.
} kraken_gpio_config_t;

/// @brief Types of multiplexers.
typedef enum kraken_mux_type : uint8_t {
    KRAKEN_MUX_TYPE_I2C,///< I2C multiplexer.
    KRAKEN_MUX_TYPE_SPI ///< SPI multiplexer.
} kraken_mux_type_t;

/// @brief Callback function type for updating I2C multiplexer state.
/// @param[in] fd The file descriptor for the I2C bus.
/// @param[in,out] shadow_memory A pointer to the shadow memory for the device.
/// @param[in] ios A pointer to an array of IO handles.
/// @param[in] io_count The number of IO handles in the array.
/// @return KRAKEN_OK on success, or an error code on failure.
typedef kraken_error_t (*pfn_kraken_i2c_mux_state_update)(int fd, void* shadow_memory, kraken_io_handle_t* ios,
                                                          size_t io_count, uint64_t mask);

/// @brief Callback function type for initializing I2C multiplexer state.
/// @param[in] fd The file descriptor for the I2C bus.
/// @param[out] shadow_memory A pointer to the shadow memory for the device.
/// @param[in] ios A pointer to an array of IO handles.
/// @param[in] io_count The number of IO handles in the array.
/// @return KRAKEN_OK on success, or an error code on failure.
typedef kraken_error_t (*pfn_kraken_i2c_mux_state_init)(int fd, void* shadow_memory, kraken_io_handle_t* ios,
                                                        size_t io_count, uint64_t mask);

/// @brief Configuration for an I2C multiplexer.
typedef struct kraken_i2c_mux_config {
    kraken_mux_type_t type;                          ///< Type of the multiplexer.
    kraken_pin_config_t* pins;                       ///< Pointer to pin configurations.
    size_t pin_count;                                ///< Number of pins.
    char* alias;                                     ///< Alias for the device.
    char* bus;                                       ///< I2C bus path.
    kraken_i2c_address_t address;                    ///< I2C device address.
    size_t shadow_memory_size;                       ///< Size of the shadow memory in bytes.
    pfn_kraken_i2c_mux_state_update pfn_state_update;///< State update function.
    pfn_kraken_i2c_mux_state_init pfn_state_init;    ///< State initialization function.
} kraken_i2c_mux_config_t;

/// @brief Callback function type for updating SPI multiplexer state.
/// @param[in] base_address The base address of the SPI device.
/// @param[in,out] shadow_memory A pointer to the shadow memory for the device.
/// @param[in] ios A pointer to an array of IO handles.
/// @param[in] io_count The number of IO handles in the array.
/// @return KRAKEN_OK on success, or an error code on failure.
typedef kraken_error_t (*pfn_kraken_spi_mux_state_update)(void* base_address, void* shadow_memory,
                                                          kraken_io_handle_t* ios, size_t io_count, uint64_t mask);

/// @brief Callback function type for initializing SPI multiplexer state.
/// @param[in] base_address The base address of the SPI device.
/// @param[out] shadow_memory A pointer to the shadow memory for the device.
/// @param[in] ios A pointer to an array of IO handles.
/// @param[in] io_count The number of IO handles in the array.
/// @return KRAKEN_OK on success, or an error code on failure.
typedef kraken_error_t (*pfn_kraken_spi_mux_state_init)(void* base_address, void* shadow_memory,
                                                        kraken_io_handle_t* ios, size_t io_count, uint64_t mask);

/// @brief Configuration for an SPI multiplexer.
typedef struct kraken_spi_mux_config {
    kraken_mux_type_t type;                          ///< Type of the multiplexer.
    kraken_pin_config_t* pins;                       ///< Pointer to pin configurations.
    size_t pin_count;                                ///< Number of pins.
    char* alias;                                     ///< Alias for the device.
    char* device;                                    ///< Path to the SPI device file.
    pfn_kraken_spi_mux_state_update pfn_state_update;///< State update function.
    pfn_kraken_spi_mux_state_init pfn_state_init;    ///< State initialization function.
} kraken_spi_mux_config_t;

/// @brief General configuration for a multiplexer.
typedef struct kraken_mux_config {
    union {
        kraken_mux_type_t type;     ///< Type of the multiplexer.
        kraken_i2c_mux_config_t i2c;///< I2C multiplexer configuration.
        kraken_spi_mux_config_t spi;///< SPI multiplexer configuration.
    };
} kraken_mux_config_t;

/// @brief Configuration for a Kraken board.
typedef struct kraken_board_config {
    kraken_gpio_config_t* gpio_config;///< GPIO configuration.
    kraken_mux_config_t** mux_configs;///< Array of multiplexer configurations.
    size_t mux_count;                 ///< Number of multiplexer configurations.
    char* flash_device;               ///< Path to the flash device file.
} kraken_board_config_t;

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_CONFIG_H