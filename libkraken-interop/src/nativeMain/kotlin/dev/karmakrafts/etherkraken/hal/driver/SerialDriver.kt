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

package dev.karmakrafts.etherkraken.hal.driver

import co.touchlab.stately.collections.ConcurrentMutableList
import dev.karmakrafts.etherkraken.hal.IO
import dev.karmakrafts.etherkraken.hal.Port
import platform.posix.usleep
import kotlin.concurrent.atomics.AtomicInt
import kotlin.concurrent.atomics.fetchAndIncrement

class SerialDriver( // @formatter:off
    port: Port,
    val dataDevicePin: UInt,
    val clockDevicePin: UInt
) : Driver(port) { // @formatter:on
    companion object {
        private const val LAST_BIT: Int = UByte.SIZE_BITS - 1
    }

    private val ios: List<IO> = port.enumerateIOs()
    private val dataIo: IO = ios.first { io -> io.devicePin == dataDevicePin }
    private val clockIo: IO = ios.first { io -> io.devicePin == clockDevicePin }

    private val queue: ConcurrentMutableList<UByte> = ConcurrentMutableList()
    private val currentByte: AtomicInt = AtomicInt(0)
    private val currentBit: AtomicInt = AtomicInt(LAST_BIT)
    private val tick: AtomicInt = AtomicInt(0)

    fun write(value: UByte) {
        queue += value
    }

    fun write(value: String) {
        value.encodeToByteArray().toUByteArray().forEach(::write)
    }

    fun waitUntilEmpty() {
        while (queue.isNotEmpty()) usleep(100000U)
        while (currentBit.load() < LAST_BIT) usleep(100000U)
    }

    override fun tick(): ULong {
        val currentBit = currentBit.load()
        var currentByte = currentByte.load().toUInt()
        if (currentBit == LAST_BIT) {
            currentByte = queue.removeFirstOrNull()?.toUInt() ?: return 0UL
            this.currentByte.store(currentByte.toInt())
            this.currentBit.store(0)
        }
        val clockRE = tick.fetchAndIncrement() and 0b1 == 0b1
        clockIo.state = clockRE
        if (clockRE) {
            dataIo.state = (currentByte shr currentBit) and 0b1U == 0b1U
            this.currentBit.fetchAndIncrement()
        }
        return ULong.MAX_VALUE
    }
}