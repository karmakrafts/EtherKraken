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

typedef enum bcm2835_pin : uint32_t {
    BCM2835_PIN_BCM0,
    BCM2835_PIN_BCM1,
    BCM2835_PIN_BCM2,
    BCM2835_PIN_BCM3,
    BCM2835_PIN_BCM4,
    BCM2835_PIN_BCM5,
    BCM2835_PIN_BCM6,
    BCM2835_PIN_BCM7,
    BCM2835_PIN_BCM8,
    BCM2835_PIN_BCM9,
    BCM2835_PIN_BCM10,
    BCM2835_PIN_BCM11,
    BCM2835_PIN_BCM12,
    BCM2835_PIN_BCM13,
    BCM2835_PIN_BCM14,
    BCM2835_PIN_BCM15,
    BCM2835_PIN_BCM16,
    BCM2835_PIN_BCM17,
    BCM2835_PIN_BCM18,
    BCM2835_PIN_BCM19,
    BCM2835_PIN_BCM20,
    BCM2835_PIN_BCM21,
    BCM2835_PIN_BCM22,
    BCM2835_PIN_BCM23,
    BCM2835_PIN_BCM24,
    BCM2835_PIN_BCM25,
    BCM2835_PIN_BCM26,
    BCM2835_PIN_BCM27,
    BCM2835_PIN_BCM28,
    BCM2835_PIN_BCM29,
    BCM2835_PIN_BCM30,
    BCM2835_PIN_BCM31,
    BCM2835_PIN_BCM32,
    BCM2835_PIN_BCM33,
    BCM2835_PIN_BCM34,
    BCM2835_PIN_BCM35,
    BCM2835_PIN_BCM36,
    BCM2835_PIN_BCM37,
    BCM2835_PIN_BCM38,
    BCM2835_PIN_BCM39,
    BCM2835_PIN_BCM40,
    BCM2835_PIN_BCM41,
    BCM2835_PIN_BCM42,
    BCM2835_PIN_BCM43,
    BCM2835_PIN_BCM44,
    BCM2835_PIN_BCM45,
    BCM2835_PIN_BCM46,
    BCM2835_PIN_BCM47,
    BCM2835_PIN_BCM48,
    BCM2835_PIN_BCM49,
    BCM2835_PIN_BCM50,
    BCM2835_PIN_BCM51,
    BCM2835_PIN_BCM52,
    BCM2835_PIN_BCM53
} bcm2835_pin_t;

typedef enum bcm2835_pud : uint8_t {
    BCM2825_PUD_OFF = 0b00,
    BCM2825_PUD_PULL_DOWN = 0b01,
    BCM2825_PUD_PULL_UP = 0b10
} bcm2835_pud_t;

typedef enum bcm2835_fsel : uint8_t {
    BCM2825_FSEL_IN = 0b000,
    BCM2825_FSEL_OUT = 0b001,
    BCM2825_FSEL_ALT0 = 0b100,
    BCM2825_FSEL_ALT1 = 0b101,
    BCM2825_FSEL_ALT2 = 0b110,
    BCM2825_FSEL_ALT3 = 0b111,
    BCM2825_FSEL_ALT4 = 0b011,
    BCM2825_FSEL_ALT5 = 0b010
} bcm2835_fsel_t;

typedef struct __attribute__((packed)) _bcm2835_gppud_t {//NOLINT
    bcm2835_pud_t pud : 2;
    uint32_t _reserved : 30;
} bcm2835_gppud_t;

typedef struct __attribute__((packed)) _bcm2835_gppinreg_t {//NOLINT
    union __attribute__((packed)) {
        struct __attribute__((packed)) {
            kraken_bool_t pin0 : 1;
            kraken_bool_t pin1 : 1;
            kraken_bool_t pin2 : 1;
            kraken_bool_t pin3 : 1;
            kraken_bool_t pin4 : 1;
            kraken_bool_t pin5 : 1;
            kraken_bool_t pin6 : 1;
            kraken_bool_t pin7 : 1;
            kraken_bool_t pin8 : 1;
            kraken_bool_t pin9 : 1;
            kraken_bool_t pin10 : 1;
            kraken_bool_t pin11 : 1;
            kraken_bool_t pin12 : 1;
            kraken_bool_t pin13 : 1;
            kraken_bool_t pin14 : 1;
            kraken_bool_t pin15 : 1;
            kraken_bool_t pin16 : 1;
            kraken_bool_t pin17 : 1;
            kraken_bool_t pin18 : 1;
            kraken_bool_t pin19 : 1;
            kraken_bool_t pin20 : 1;
            kraken_bool_t pin21 : 1;
            kraken_bool_t pin22 : 1;
            kraken_bool_t pin23 : 1;
            kraken_bool_t pin24 : 1;
            kraken_bool_t pin25 : 1;
            kraken_bool_t pin26 : 1;
            kraken_bool_t pin27 : 1;
            kraken_bool_t pin28 : 1;
            kraken_bool_t pin29 : 1;
            kraken_bool_t pin30 : 1;
            kraken_bool_t pin31 : 1;
        };
        uint32_t value;
    };
} bcm2835_gppinreg_t;

typedef struct __attribute__((packed)) _bcm2835_gpfsel_t {// NOLINT
    union __attribute__((packed)) {
        struct __attribute__((packed)) {
            bcm2835_fsel_t fsel0 : 3;
            bcm2835_fsel_t fsel1 : 3;
            bcm2835_fsel_t fsel2 : 3;
            bcm2835_fsel_t fsel3 : 3;
            bcm2835_fsel_t fsel4 : 3;
            bcm2835_fsel_t fsel5 : 3;
            bcm2835_fsel_t fsel6 : 3;
            bcm2835_fsel_t fsel7 : 3;
            bcm2835_fsel_t fsel8 : 3;
            bcm2835_fsel_t fsel9 : 3;
            uint8_t _reserved : 2;
        };
        uint32_t value;
    };
} bcm2835_gpfsel_t;

typedef struct __attribute__((packed)) _bcm2835_gpio_t {// NOLINT
    bcm2835_gpfsel_t gpfsel0;                           // FSEL0 - FSEL9
    bcm2835_gpfsel_t gpfsel1;                           // FSEL10 - FSEL19
    bcm2835_gpfsel_t gpfsel2;                           // FSEL20 - FSEL29
    bcm2835_gpfsel_t gpfsel3;                           // FSEL30 - FSEL39
    bcm2835_gpfsel_t gpfsel4;                           // FSEL40 - FSEL49
    bcm2835_gpfsel_t gpfsel5;                           // FSEL50 - FSEL53
    uint32_t _reserved0;
    bcm2835_gppinreg_t gpset0;
    bcm2835_gppinreg_t gpset1;
    uint32_t _reserved1;
    bcm2835_gppinreg_t gpclr0;
    bcm2835_gppinreg_t gpclr1;
    uint32_t _reserved2;
    bcm2835_gppinreg_t gplev0;
    bcm2835_gppinreg_t gplev1;
    uint32_t _reserved3;
    bcm2835_gppinreg_t gpeds0;
    bcm2835_gppinreg_t gpeds1;
    uint32_t _reserved4;
    bcm2835_gppinreg_t gpren0;
    bcm2835_gppinreg_t gpren1;
    uint32_t _reserved5;
    bcm2835_gppinreg_t gpfen0;
    bcm2835_gppinreg_t gpfen1;
    uint32_t _reserved6;
    bcm2835_gppinreg_t gphen0;
    bcm2835_gppinreg_t gphen1;
    uint32_t _reserved7;
    bcm2835_gppinreg_t gplen0;
    bcm2835_gppinreg_t gplen1;
    uint32_t _reserved8;
    bcm2835_gppinreg_t gparen0;
    bcm2835_gppinreg_t gparen1;
    uint32_t _reserved9;
    bcm2835_gppinreg_t gpafen0;
    bcm2835_gppinreg_t gpafen1;
    uint32_t _reserved10;
    bcm2835_gppud_t gppud;
    bcm2835_gppinreg_t gppudclk0;
    bcm2835_gppinreg_t gppudclk1;
    uint32_t _reserved11;
    uint8_t test : 4;
} bcm2835_gpio_t;

KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_update(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                       size_t io_count);

KRAKEN_EXPORT kraken_error_t bcm2835_gpio_state_init(void* base_address, void* shadow_memory, kraken_io_handle_t* ios,
                                                     size_t io_count);

KRAKEN_API_END

#endif//LIBKRAKEN_BCM2835_H