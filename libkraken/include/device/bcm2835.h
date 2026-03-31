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

#ifndef LIBKRAKEN_BCM2835_H
#define LIBKRAKEN_BCM2835_H

#include "kraken_api.h"
#include "kraken_error.h"
#include "kraken_handles.h"

KRAKEN_API_BEGIN

///
/// See https://pip-assets.raspberrypi.com/categories/579-raspberry-pi-zero/documents/RP-008249-DS-1-bcm2835-peripherals.pdf?disposition=inline
/// for implementation details.
///

///
/// BCM2835 GPIO pins.
///
typedef enum bcm2835_pin : uint32_t {
    BCM2835_PIN_BCM0, ///< GPIO 0
    BCM2835_PIN_BCM1, ///< GPIO 1
    BCM2835_PIN_BCM2, ///< GPIO 2
    BCM2835_PIN_BCM3, ///< GPIO 3
    BCM2835_PIN_BCM4, ///< GPIO 4
    BCM2835_PIN_BCM5, ///< GPIO 5
    BCM2835_PIN_BCM6, ///< GPIO 6
    BCM2835_PIN_BCM7, ///< GPIO 7
    BCM2835_PIN_BCM8, ///< GPIO 8
    BCM2835_PIN_BCM9, ///< GPIO 9
    BCM2835_PIN_BCM10,///< GPIO 10
    BCM2835_PIN_BCM11,///< GPIO 11
    BCM2835_PIN_BCM12,///< GPIO 12
    BCM2835_PIN_BCM13,///< GPIO 13
    BCM2835_PIN_BCM14,///< GPIO 14
    BCM2835_PIN_BCM15,///< GPIO 15
    BCM2835_PIN_BCM16,///< GPIO 16
    BCM2835_PIN_BCM17,///< GPIO 17
    BCM2835_PIN_BCM18,///< GPIO 18
    BCM2835_PIN_BCM19,///< GPIO 19
    BCM2835_PIN_BCM20,///< GPIO 20
    BCM2835_PIN_BCM21,///< GPIO 21
    BCM2835_PIN_BCM22,///< GPIO 22
    BCM2835_PIN_BCM23,///< GPIO 24
    BCM2835_PIN_BCM24,///< GPIO 24
    BCM2835_PIN_BCM25,///< GPIO 25
    BCM2835_PIN_BCM26,///< GPIO 26
    BCM2835_PIN_BCM27,///< GPIO 27
    BCM2835_PIN_BCM28,///< GPIO 28
    BCM2835_PIN_BCM29,///< GPIO 29
    BCM2835_PIN_BCM30,///< GPIO 30
    BCM2835_PIN_BCM31,///< GPIO 31
    BCM2835_PIN_BCM32,///< GPIO 32
    BCM2835_PIN_BCM33,///< GPIO 33
    BCM2835_PIN_BCM34,///< GPIO 34
    BCM2835_PIN_BCM35,///< GPIO 35
    BCM2835_PIN_BCM36,///< GPIO 36
    BCM2835_PIN_BCM37,///< GPIO 37
    BCM2835_PIN_BCM38,///< GPIO 38
    BCM2835_PIN_BCM39,///< GPIO 39
    BCM2835_PIN_BCM40,///< GPIO 40
    BCM2835_PIN_BCM41,///< GPIO 41
    BCM2835_PIN_BCM42,///< GPIO 42
    BCM2835_PIN_BCM43,///< GPIO 43
    BCM2835_PIN_BCM44,///< GPIO 44
    BCM2835_PIN_BCM45,///< GPIO 45
    BCM2835_PIN_BCM46,///< GPIO 46
    BCM2835_PIN_BCM47,///< GPIO 47
    BCM2835_PIN_BCM48,///< GPIO 48
    BCM2835_PIN_BCM49,///< GPIO 49
    BCM2835_PIN_BCM50,///< GPIO 50
    BCM2835_PIN_BCM51,///< GPIO 51
    BCM2835_PIN_BCM52,///< GPIO 52
    BCM2835_PIN_BCM53 ///< GPIO 53
} bcm2835_pin_t;

///
/// BCM2835 GPIO pull-up/down control.
///
typedef enum bcm2835_pud : uint8_t {
    BCM2825_PUD_OFF = 0b00,      ///< Off – disable pull-up/down
    BCM2825_PUD_PULL_DOWN = 0b01,///< Pull Down – enable pull-down control
    BCM2825_PUD_PULL_UP = 0b10   ///< Pull Up – enable pull-up control
} bcm2835_pud_t;

///
/// BCM2835 GPIO function selection.
///
typedef enum bcm2835_fsel : uint8_t {
    BCM2825_FSEL_IN = 0b000,  ///< Input
    BCM2825_FSEL_OUT = 0b001, ///< Output
    BCM2825_FSEL_ALT0 = 0b100,///< Alternative function 0
    BCM2825_FSEL_ALT1 = 0b101,///< Alternative function 1
    BCM2825_FSEL_ALT2 = 0b110,///< Alternative function 2
    BCM2825_FSEL_ALT3 = 0b111,///< Alternative function 3
    BCM2825_FSEL_ALT4 = 0b011,///< Alternative function 4
    BCM2825_FSEL_ALT5 = 0b010 ///< Alternative function 5
} bcm2835_fsel_t;

///
/// BCM2835 GPIO Pull-up/down Register (GPPUD).
///
typedef struct __attribute__((packed)) _bcm2835_gppud_t {//NOLINT
    bcm2835_pud_t pud : 2;                               ///< GPIO Pin Pull-up/down
    uint32_t _reserved : 30;                             ///< Reserved
} bcm2835_gppud_t;

///
/// BCM2835 GPIO Pin Register.
///
typedef struct __attribute__((packed)) _bcm2835_gppinreg_t {//NOLINT
    union {
        struct __attribute__((packed)) {
            kraken_bool_t pin0 : 1; ///< GPIO 0
            kraken_bool_t pin1 : 1; ///< GPIO 1
            kraken_bool_t pin2 : 1; ///< GPIO 2
            kraken_bool_t pin3 : 1; ///< GPIO 3
            kraken_bool_t pin4 : 1; ///< GPIO 4
            kraken_bool_t pin5 : 1; ///< GPIO 5
            kraken_bool_t pin6 : 1; ///< GPIO 6
            kraken_bool_t pin7 : 1; ///< GPIO 7
            kraken_bool_t pin8 : 1; ///< GPIO 8
            kraken_bool_t pin9 : 1; ///< GPIO 9
            kraken_bool_t pin10 : 1;///< GPIO 10
            kraken_bool_t pin11 : 1;///< GPIO 11
            kraken_bool_t pin12 : 1;///< GPIO 12
            kraken_bool_t pin13 : 1;///< GPIO 13
            kraken_bool_t pin14 : 1;///< GPIO 14
            kraken_bool_t pin15 : 1;///< GPIO 15
            kraken_bool_t pin16 : 1;///< GPIO 16
            kraken_bool_t pin17 : 1;///< GPIO 17
            kraken_bool_t pin18 : 1;///< GPIO 18
            kraken_bool_t pin19 : 1;///< GPIO 19
            kraken_bool_t pin20 : 1;///< GPIO 20
            kraken_bool_t pin21 : 1;///< GPIO 21
            kraken_bool_t pin22 : 1;///< GPIO 22
            kraken_bool_t pin23 : 1;///< GPIO 23
            kraken_bool_t pin24 : 1;///< GPIO 24
            kraken_bool_t pin25 : 1;///< GPIO 25
            kraken_bool_t pin26 : 1;///< GPIO 26
            kraken_bool_t pin27 : 1;///< GPIO 27
            kraken_bool_t pin28 : 1;///< GPIO 28
            kraken_bool_t pin29 : 1;///< GPIO 29
            kraken_bool_t pin30 : 1;///< GPIO 30
            kraken_bool_t pin31 : 1;///< GPIO 31
        };
        uint32_t value;///< Raw register value
    };
} bcm2835_gppinreg_t;

///
/// BCM2835 GPIO Function Select Register (GPFSEL).
///
typedef struct __attribute__((packed)) _bcm2835_gpfsel_t {// NOLINT
    union {
        struct __attribute__((packed)) {
            bcm2835_fsel_t fsel0 : 3;///< Function select 0
            bcm2835_fsel_t fsel1 : 3;///< Function select 1
            bcm2835_fsel_t fsel2 : 3;///< Function select 2
            bcm2835_fsel_t fsel3 : 3;///< Function select 3
            bcm2835_fsel_t fsel4 : 3;///< Function select 4
            bcm2835_fsel_t fsel5 : 3;///< Function select 5
            bcm2835_fsel_t fsel6 : 3;///< Function select 6
            bcm2835_fsel_t fsel7 : 3;///< Function select 7
            bcm2835_fsel_t fsel8 : 3;///< Function select 8
            bcm2835_fsel_t fsel9 : 3;///< Function select 9
            uint8_t _reserved : 2;   ///< Reserved
        };
        uint32_t value;///< Raw register value
    };
} bcm2835_gpfsel_t;

///
/// BCM2835 GPIO register layout.
///
typedef struct __attribute__((packed)) _bcm2835_gpio_t {// NOLINT
    bcm2835_gpfsel_t gpfsel0;                           ///< GPIO Function Select 0
    bcm2835_gpfsel_t gpfsel1;                           ///< GPIO Function Select 1
    bcm2835_gpfsel_t gpfsel2;                           ///< GPIO Function Select 2
    bcm2835_gpfsel_t gpfsel3;                           ///< GPIO Function Select 3
    bcm2835_gpfsel_t gpfsel4;                           ///< GPIO Function Select 4
    bcm2835_gpfsel_t gpfsel5;                           ///< GPIO Function Select 5
    uint32_t _reserved0;                                ///< Reserved
    bcm2835_gppinreg_t gpset0;                          ///< GPIO Pin Output Set 0
    bcm2835_gppinreg_t gpset1;                          ///< GPIO Pin Output Set 1
    uint32_t _reserved1;                                ///< Reserved
    bcm2835_gppinreg_t gpclr0;                          ///< GPIO Pin Output Clear 0
    bcm2835_gppinreg_t gpclr1;                          ///< GPIO Pin Output Clear 1
    uint32_t _reserved2;                                ///< Reserved
    bcm2835_gppinreg_t gplev0;                          ///< GPIO Pin Level 0
    bcm2835_gppinreg_t gplev1;                          ///< GPIO Pin Level 1
    uint32_t _reserved3;                                ///< Reserved
    bcm2835_gppinreg_t gpeds0;                          ///< GPIO Pin Event Detect Status 0
    bcm2835_gppinreg_t gpeds1;                          ///< GPIO Pin Event Detect Status 1
    uint32_t _reserved4;                                ///< Reserved
    bcm2835_gppinreg_t gpren0;                          ///< GPIO Pin Rising Edge Detect Enable 0
    bcm2835_gppinreg_t gpren1;                          ///< GPIO Pin Rising Edge Detect Enable 1
    uint32_t _reserved5;                                ///< Reserved
    bcm2835_gppinreg_t gpfen0;                          ///< GPIO Pin Falling Edge Detect Enable 0
    bcm2835_gppinreg_t gpfen1;                          ///< GPIO Pin Falling Edge Detect Enable 1
    uint32_t _reserved6;                                ///< Reserved
    bcm2835_gppinreg_t gphen0;                          ///< GPIO Pin High Detect Enable 0
    bcm2835_gppinreg_t gphen1;                          ///< GPIO Pin High Detect Enable 1
    uint32_t _reserved7;                                ///< Reserved
    bcm2835_gppinreg_t gplen0;                          ///< GPIO Pin Low Detect Enable 0
    bcm2835_gppinreg_t gplen1;                          ///< GPIO Pin Low Detect Enable 1
    uint32_t _reserved8;                                ///< Reserved
    bcm2835_gppinreg_t gparen0;                         ///< GPIO Pin Async. Rising Edge Detect 0
    bcm2835_gppinreg_t gparen1;                         ///< GPIO Pin Async. Rising Edge Detect 1
    uint32_t _reserved9;                                ///< Reserved
    bcm2835_gppinreg_t gpafen0;                         ///< GPIO Pin Async. Falling Edge Detect 0
    bcm2835_gppinreg_t gpafen1;                         ///< GPIO Pin Async. Falling Edge Detect 1
    uint32_t _reserved10;                               ///< Reserved
    bcm2835_gppud_t gppud;                              ///< GPIO Pin Pull-up/down Enable
    bcm2835_gppinreg_t gppudclk0;                       ///< GPIO Pin Pull-up/down Enable Clock 0
    bcm2835_gppinreg_t gppudclk1;                       ///< GPIO Pin Pull-up/down Enable Clock 1
    uint32_t _reserved11;                               ///< Reserved
    uint8_t test : 4;                                   ///< Test
} bcm2835_gpio_t;

///
/// Updates the GPIO state from the base address and shadow memory.
///
/// @param base_address The base address of the GPIO registers.
/// @param shadow_memory The shadow memory to update from.
/// @param ios The IO handles to update.
/// @param io_count The number of IO handles.
/// @return KRAKEN_SUCCESS on success, or an error code on failure.
///
KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_update(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                       size_t io_count);

///
/// Initializes the GPIO state.
///
/// @param base_address The base address of the GPIO registers.
/// @param shadow_memory The shadow memory to initialize.
/// @param ios The IO handles to initialize.
/// @param io_count The number of IO handles.
/// @return KRAKEN_SUCCESS on success, or an error code on failure.
///
KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_init(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                     size_t io_count);

KRAKEN_API_END

#endif//LIBKRAKEN_BCM2835_H