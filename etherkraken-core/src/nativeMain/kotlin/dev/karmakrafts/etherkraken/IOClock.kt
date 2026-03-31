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

package dev.karmakrafts.etherkraken

import co.touchlab.stately.collections.ConcurrentMutableList
import dev.karmakrafts.etherkraken.hal.check
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_cpu_get_counter
import platform.posix.uint64_tVar

class IOClock(
    frequency: Double
) {
    val periodCycles: ULong = (IODispatcher.systemFrequency.toDouble() / frequency + 0.5).toULong()

    internal var nextEventCycles: ULong = memScoped {
        val value = alloc<uint64_tVar>()
        kraken_cpu_get_counter(value.ptr).check()
        value.value - periodCycles
    }

    internal val drivers: ConcurrentMutableList<IODriver> = ConcurrentMutableList()

    operator fun plusAssign(driver: IODriver) {
        drivers += driver
    }

    operator fun minusAssign(driver: IODriver) {
        drivers -= driver
    }

    fun tick() {
        for (driver in drivers.toList()) driver.tick()
    }
}