/*
 * Copyright 2026 Karma Krafts
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

@file:OptIn(ExperimentalForeignApi::class)

package dev.karmakrafts.etherkraken.selftest

import dev.karmakrafts.etherkraken.hal.Board
import dev.karmakrafts.etherkraken.hal.HAL
import dev.karmakrafts.etherkraken.hal.config.BoardConfig
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.sizeOf
import kotlinx.cinterop.toLong
import libkraken.bcm2835_gpio_t
import libkraken.bcm2835_pin_t

fun main() {
    println("==================== RUNNING SELFTEST ====================")
    HAL.init() // Register logging callbacks for debugging and initialize internal state
    BoardConfig {
        gpio {
            deviceTreeEntry = "/proc/device-tree/soc/gpiomem"
            deviceType = "bcm2835"
            device = "/dev/gpiomem"
            registersSize = sizeOf<bcm2835_gpio_t>()
            pin(bcm2835_pin_t.BCM2835_PIN_BCM13.value, 8U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM25.value, 10U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM24.value, 12U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM23.value, 14U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM22.value, 16U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM26.value, 18U)
        }
    }.use { config ->
        Board.create(config).use { board ->
            println("Created HAL board instance at 0x${board.handle.toLong().toHexString()}")
            // TODO...
        }
    }
    println("==========================================================")
}