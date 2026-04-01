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
import dev.karmakrafts.etherkraken.hal.Clock
import dev.karmakrafts.etherkraken.hal.Dispatcher
import dev.karmakrafts.etherkraken.hal.Driver
import dev.karmakrafts.etherkraken.hal.IO
import dev.karmakrafts.etherkraken.hal.Port
import dev.karmakrafts.etherkraken.hal.config.BoardConfig
import dev.karmakrafts.etherkraken.hal.config.DefaultGPIOType
import kotlinx.cinterop.ExperimentalForeignApi
import libkraken.bcm2835_pin_t
import libkraken.kraken_io_mode_t
import libkraken.kraken_port_type_t
import platform.posix.sched_yield
import platform.posix.usleep
import kotlin.concurrent.atomics.AtomicInt
import kotlin.concurrent.atomics.fetchAndIncrement

fun main() {
    println("==================== RUNNING SELFTEST ====================")
    val dispatcher = Dispatcher(2)
    Board.create(BoardConfig {
        gpio(DefaultGPIOType.BCM2835) {
            deviceTreeEntry("/proc/device-tree/soc/gpiomem")
            device("/dev/gpiomem")
            pin(bcm2835_pin_t.BCM2835_PIN_BCM2.value, 1000U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM3.value, 1001U)
            pin(bcm2835_pin_t.BCM2835_PIN_BCM4.value, 1002U)
        }
    }).use { board ->
        val ports = ArrayList<Port>()
        board.enumeratePorts(ports)
        for (port in ports) {
            if (port.type == kraken_port_type_t.KRAKEN_PORT_TYPE_GPIO) {
                println("Found GPIO port, initializing..")
                val ios = ArrayList<IO>()
                port.enumerateIOs(ios)
                for (io in ios) {
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

                // Driver based realtime IO, create a 50Hz square-wave
                println("Testing realtime driver..")
                Clock(50.0).use { clock ->
                    dispatcher += clock
                    val maxCount = 500
                    val count = AtomicInt(0)
                    Driver(port) { ios ->
                        val tick = count.fetchAndIncrement()
                        if (tick >= maxCount) return@Driver 0UL // don't commit any port updates
                        for (io in ios) io.state = tick and 1 == 0
                        ULong.MAX_VALUE // commit port updates for all IOs
                    }.use { driver ->
                        clock += driver
                        while (count.load() < maxCount) sched_yield()
                    }
                }
            }
        }
    }
    dispatcher.close()
    println("==========================================================")
}