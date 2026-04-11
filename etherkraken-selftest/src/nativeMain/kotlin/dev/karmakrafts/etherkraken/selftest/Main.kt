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
import dev.karmakrafts.etherkraken.hal.IO
import dev.karmakrafts.etherkraken.hal.Port
import dev.karmakrafts.etherkraken.hal.config.BoardConfig
import dev.karmakrafts.etherkraken.hal.config.DefaultGPIOType
import dev.karmakrafts.etherkraken.hal.driver.Driver
import dev.karmakrafts.etherkraken.hal.driver.SerialTxDriver
import dev.karmakrafts.etherkraken.hal.driver.UartTxDriver
import kotlinx.cinterop.ExperimentalForeignApi
import libkraken.bcm2835_pin_t
import libkraken.kraken_io_mode_t
import libkraken.kraken_port_type_t
import platform.posix.sleep
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

                //println("Testing outputs..")
                //var lastIo: IO? = null
                //for (i in 0..<30) {
                //    lastIo?.state = false
                //    port.update()
                //    val io = ios[i % 3]
                //    io.state = true
                //    port.update()
                //    lastIo = io
                //    usleep(250000U)
                //}
                //lastIo?.state = false
                //port.update()

                //println("Testing SerialTxDriver..")
                //Clock(25.0).use { clock ->
                //    dispatcher += clock
                //    SerialTxDriver(
                //        port, bcm2835_pin_t.BCM2835_PIN_BCM2.value, bcm2835_pin_t.BCM2835_PIN_BCM3.value
                //    ).use { driver ->
                //        clock += driver
                //        driver.write("HELLO WORLD! THIS IS SERIAL OUTPUT!")
                //        driver.wait()
                //        clock -= driver
                //    }
                //    dispatcher -= clock
                //}

                println("Clearing outputs..")
                for (io in ios) io.state = false
                port.update()

                println("Testing UartTxDriver..")
                Clock(9600.0).use { clock ->
                    dispatcher += clock
                    UartTxDriver(
                        port, bcm2835_pin_t.BCM2835_PIN_BCM4.value
                    ).use { driver ->
                        // First transfer to TX buffer
                        driver.write("HELLO WORLD! THIS IS UART OUTPUT!\n".repeat(20))
                        // Start bit banging on the wire
                        clock += driver
                        // Wait until internal TX buffer is empty and last bit was sent
                        driver.wait()
                        // Stop bit banging on the wire
                        clock -= driver
                    }
                    dispatcher -= clock
                }

                println("Clearing outputs..")
                for (io in ios) io.state = false
                port.update()

                //println("Testing parallel clocks..")

                //val clock1 = Clock(50.0)
                //dispatcher += clock1
                //val io1 = ios.first { io -> io.devicePin == bcm2835_pin_t.BCM2835_PIN_BCM2.value }
                //val tick1 = AtomicInt(0)
                //val driver1 = Driver(port) {
                //    io1.state = (tick1.fetchAndIncrement() and 1 == 0)
                //    0b1UL shl bcm2835_pin_t.BCM2835_PIN_BCM2.value.toInt()
                //}
                //clock1 += driver1

                //val clock2 = Clock(10.0)
                //dispatcher += clock2
                //val io2 = ios.first { io -> io.devicePin == bcm2835_pin_t.BCM2835_PIN_BCM3.value }
                //val tick2 = AtomicInt(0)
                //val driver2 = Driver(port) {
                //    io2.state = (tick2.fetchAndIncrement() and 1 == 0)
                //    0b1UL shl bcm2835_pin_t.BCM2835_PIN_BCM3.value.toInt()
                //}
                //clock2 += driver2

                //sleep(5U)

                //clock1 -= driver1
                //driver1.close()
                //dispatcher -= clock1
                //clock1.close()

                //clock2 -= driver2
                //driver2.close()
                //dispatcher -= clock2
                //clock2.close()

                //println("Clearing outputs..")
                //for (io in ios) io.state = false
                //port.update()
            }
        }
    }
    dispatcher.close()
    println("==========================================================")
}