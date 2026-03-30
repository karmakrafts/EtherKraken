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
import dev.karmakrafts.etherkraken.hal.IO
import dev.karmakrafts.etherkraken.hal.config.BoardConfig
import dev.karmakrafts.etherkraken.hal.config.DefaultGPIOType
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.toLong
import libkraken.bcm2835_pin_t
import libkraken.kraken_io_mode_t
import libkraken.kraken_port_type_t
import platform.posix.usleep

fun main() {
    println("==================== RUNNING SELFTEST ====================")
    HAL.init() // Register logging callbacks for debugging and initialize internal state
    Board.create(BoardConfig {
        gpio(DefaultGPIOType.BCM2835) {
            deviceTreeEntry("/proc/device-tree/soc/gpiomem")
            device("/dev/gpiomem")
            pin(bcm2835_pin_t.BCM2835_PIN_BCM2.value, 1000U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM3.value, 1001U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM4.value, 1002U)
        }
    }).use { board ->
        println("Created HAL board instance at 0x${board.handle.toLong().toHexString()}")
        for (port in board.enumeratePorts()) {
            println("Enumerating port at 0x${port.handle.toLong().toHexString()}")
            if (port.type == kraken_port_type_t.KRAKEN_PORT_TYPE_GPIO) {
                println("Found GPIO port, initializing..")
                val ios = port.enumerateIOs()
                for (io in ios) {
                    println("Enumerating IO '${io.name}' at 0x${io.handle.toLong().toHexString()}")
                    io.mode = kraken_io_mode_t.KRAKEN_IO_MODE_OUT
                }
                port.reinit()

                println("Initialized GPIO port, updating initial state")
                port.update()

                var lastIo: IO? = null
                for (i in 0..<30) {
                    lastIo?.state = false
                    port.update()
                    val io = ios[i % 3]
                    io.state = true
                    port.update()
                    lastIo = io
                    usleep(250000U)
                }
            }
        }
    }
    println("==========================================================")
}