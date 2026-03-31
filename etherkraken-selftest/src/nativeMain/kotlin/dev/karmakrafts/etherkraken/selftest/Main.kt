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

import dev.karmakrafts.etherkraken.IOClock
import dev.karmakrafts.etherkraken.IODispatcher
import dev.karmakrafts.etherkraken.IODriver
import dev.karmakrafts.etherkraken.hal.Board
import dev.karmakrafts.etherkraken.hal.IO
import dev.karmakrafts.etherkraken.hal.Port
import dev.karmakrafts.etherkraken.hal.config.BoardConfig
import dev.karmakrafts.etherkraken.hal.config.DefaultGPIOType
import dev.karmakrafts.filament.Thread
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.toLong
import libkraken.bcm2835_pin_t
import libkraken.kraken_io_mode_t
import libkraken.kraken_port_type_t
import platform.posix.usleep
import kotlin.concurrent.atomics.AtomicInt
import kotlin.concurrent.atomics.incrementAndFetch

fun main() {
    println("==================== RUNNING SELFTEST ====================")
    val dispatcher = IODispatcher(2) // use core 2
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
        val ports = ArrayList<Port>()
        board.enumeratePorts(ports)
        for (port in ports) {
            println("Enumerating port at 0x${port.handle.toLong().toHexString()}")
            if (port.type == kraken_port_type_t.KRAKEN_PORT_TYPE_GPIO) {
                println("Found GPIO port, initializing..")
                val ios = ArrayList<IO>()
                port.enumerateIOs(ios)
                for (io in ios) {
                    println("Enumerating IO '${io.name}' at 0x${io.handle.toLong().toHexString()}")
                    io.mode = kraken_io_mode_t.KRAKEN_IO_MODE_OUT
                }
                port.reinit()

                println("Initialized GPIO port, updating initial state")
                port.update()

                println("Testing outputs..")
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

                println("Starting IO driver test..")
                val count = AtomicInt(0)
                val maxCount = 300000
                val clock = IOClock(10000.0) // 10kHz
                dispatcher += clock
                val driver = IODriver(port) { ios ->
                    val tick = count.incrementAndFetch()
                    if (tick == maxCount) return@IODriver
                    ios[0].state = tick and 1 == 0
                    ios[1].state = tick and 1 != 0
                    ios[2].state = tick and 1 == 0
                }
                clock += driver
                while (count.load() < maxCount) Thread.yield()
            }
        }
    }
    dispatcher.close()
    println("==========================================================")
}