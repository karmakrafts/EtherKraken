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
import dev.karmakrafts.etherkraken.hal.config.DefaultGPIOType
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.toLong
import libkraken.bcm2835_pin_t
import libkraken.kraken_port_type_t

fun main() {
    println("==================== RUNNING SELFTEST ====================")
    HAL.init() // Register logging callbacks for debugging and initialize internal state
    BoardConfig {
        gpio(DefaultGPIOType.BCM2835) {
            deviceTreeEntry("/proc/device-tree/soc/gpiomem")
            device("/dev/gpiomem")
            pin(bcm2835_pin_t.BCM2835_PIN_BCM13.value, 8U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM25.value, 10U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM24.value, 12U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM23.value, 14U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM22.value, 16U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM26.value, 18U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM27.value, 0U) // AUX power state
            pin(bcm2835_pin_t.BCM2835_PIN_BCM47.value, 0U) // LED
        }
    }.use { config ->
        Board.create(config).use { board ->
            println("Created HAL board instance at 0x${board.handle.toLong().toHexString()}")
            for (port in board.enumeratePorts()) {
                println("Enumerating port at 0x${port.handle.toLong().toHexString()}")
                if (port.type == kraken_port_type_t.KRAKEN_PORT_TYPE_GPIO) {
                    println("Found GPIO port, initializing..")
                    port.reinit()
                    println("Initialized GPIO port, updating initial state")
                    //port.update()
                    //println("GPIO port ready!")
                }
            }
        }
    }
    println("==========================================================")
}