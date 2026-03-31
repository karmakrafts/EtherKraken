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
import dev.karmakrafts.filament.Thread
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.cValue
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_cpu_get_counter
import libkraken.kraken_cpu_get_system_frequency
import libkraken.kraken_cpu_sleep_cycles
import platform.posix.SCHED_FIFO
import platform.posix.pthread_self
import platform.posix.pthread_setschedparam
import platform.posix.uint64_tVar
import kotlin.concurrent.atomics.AtomicBoolean

class IODispatcher(coreAffinity: Int) : AutoCloseable {
    companion object {
        internal val systemFrequency: ULong = memScoped {
            val value = alloc<uint64_tVar>()
            kraken_cpu_get_system_frequency(value.ptr).check()
            value.value
        }
    }

    private val isRunning: AtomicBoolean = AtomicBoolean(true)
    private val clocks: ConcurrentMutableList<IOClock> = ConcurrentMutableList()

    private val thread: Thread = Thread( // @formatter:off
        affinity = coreAffinity,
        function = ::eventLoop
    ) // @formatter:on

    operator fun plusAssign(clock: IOClock) {
        clocks += clock
    }

    operator fun minusAssign(clock: IOClock) {
        clocks -= clock
    }

    private fun eventLoop() = memScoped {
        // Configure thread for FIFO scheduling
        pthread_setschedparam(pthread_self(), SCHED_FIFO, cValue {
            __sched_priority = 99 // realtime
        })
        // Start main event loop
        val nowCycles = alloc<uint64_tVar>()
        while (isRunning.load()) {
            kraken_cpu_get_counter(nowCycles.ptr).check()
            val nowCyclesValue = nowCycles.value
            var nextEvent = ULong.MAX_VALUE
            for (clock in clocks.toList()) {
                val periodCycles = clock.periodCycles
                if (clock.nextEventCycles <= nowCyclesValue) {
                    clock.tick()
                    clock.nextEventCycles += periodCycles
                    if (clock.nextEventCycles <= nowCyclesValue) {
                        val missed = (nowCyclesValue - clock.nextEventCycles) / periodCycles + 1UL
                        clock.nextEventCycles += missed * periodCycles
                    }
                }
                if (clock.nextEventCycles < nextEvent) {
                    nextEvent = clock.nextEventCycles
                }
            }
            if (nextEvent > nowCyclesValue) {
                kraken_cpu_sleep_cycles(nextEvent - nowCyclesValue)
            }
        }
    }

    override fun close() {
        isRunning.store(false)
        thread.join()
    }
}