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

#include <stdint.h>

///
/// See https://pip-assets.raspberrypi.com/categories/579-raspberry-pi-zero/documents/RP-008249-DS-1-bcm2835-peripherals.pdf?disposition=inline
/// for implementation details.
///

typedef uint8_t bcm2835_pud_t;
constexpr bcm2835_pud_t PUD_OFF = 0b00;
constexpr bcm2835_pud_t PUD_PULL_DOWN = 0b01;
constexpr bcm2835_pud_t PUD_PULL_UP = 0b10;

typedef uint8_t bcm2835_fsel_t;
constexpr bcm2835_fsel_t FSEL_IN = 0b000;
constexpr bcm2835_fsel_t FSEL_OUT = 0b001;
// Alternative GPIO configurations are mostly unused due to EXT port being hardwired
constexpr bcm2835_fsel_t FSEL_ALT0 = 0b100;
constexpr bcm2835_fsel_t FSEL_ALT1 = 0b101;
constexpr bcm2835_fsel_t FSEL_ALT2 = 0b110;
constexpr bcm2835_fsel_t FSEL_ALT3 = 0b111;
constexpr bcm2835_fsel_t FSEL_ALT4 = 0b011;
constexpr bcm2835_fsel_t FSEL_ALT5 = 0b010;

typedef struct _bcm2835_gppud_t {//NOLINT
    bcm2835_pud_t pud : 2;
    uint32_t _reserved : 30;
} bcm2835_gppud_t;

typedef struct _bcm2835_gppinreg_t {//NOLINT
    bool pin0 : 1;
    bool pin1 : 1;
    bool pin2 : 1;
    bool pin3 : 1;
    bool pin4 : 1;
    bool pin5 : 1;
    bool pin6 : 1;
    bool pin7 : 1;
    bool pin8 : 1;
    bool pin9 : 1;
    bool pin10 : 1;
    bool pin11 : 1;
    bool pin12 : 1;
    bool pin13 : 1;
    bool pin14 : 1;
    bool pin15 : 1;
    bool pin16 : 1;
    bool pin17 : 1;
    bool pin18 : 1;
    bool pin19 : 1;
    bool pin20 : 1;
    bool pin21 : 1;
    bool pin22 : 1;
    bool pin23 : 1;
    bool pin24 : 1;
    bool pin25 : 1;
    bool pin26 : 1;
    bool pin27 : 1;
    bool pin28 : 1;
    bool pin29 : 1;
    bool pin30 : 1;
    bool pin31 : 1;
} bcm2835_gppinreg_t;

typedef struct _bcm2835_gpfsel_t {// NOLINT
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
} bcm2835_gpfsel_t;

typedef struct _bcm2835_gpio_t {// NOLINT
    bcm2835_gpfsel_t gpfsel0;   // FSEL0 - FSEL9
    bcm2835_gpfsel_t gpfsel1;   // FSEL10 - FSEL19
    bcm2835_gpfsel_t gpfsel2;   // FSEL20 - FSEL29
    bcm2835_gpfsel_t gpfsel3;   // FSEL30 - FSEL39
    bcm2835_gpfsel_t gpfsel4;   // FSEL40 - FSEL49
    bcm2835_gpfsel_t gpfsel5;   // FSEL50 - FSEL53
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

#endif//LIBKRAKEN_BCM2835_H