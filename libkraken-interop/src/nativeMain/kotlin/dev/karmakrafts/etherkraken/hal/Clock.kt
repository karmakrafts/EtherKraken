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

package dev.karmakrafts.etherkraken.hal

import dev.karmakrafts.etherkraken.hal.driver.Driver
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_clock_create
import libkraken.kraken_clock_destroy
import libkraken.kraken_clock_handle_t
import libkraken.kraken_clock_handle_tVar
import libkraken.kraken_clock_register
import libkraken.kraken_clock_unregister

value class Clock private constructor(val handle: kraken_clock_handle_t) : AutoCloseable {
    constructor(frequency: Double) : this(memScoped {
        val handle = alloc<kraken_clock_handle_tVar>()
        kraken_clock_create(frequency, handle.ptr).check()
        checkNotNull(handle.value) { "Could not create clock" }
    })

    operator fun plusAssign(driver: Driver) {
        kraken_clock_register(handle, driver.handle).check()
    }

    operator fun minusAssign(driver: Driver) {
        kraken_clock_unregister(handle, driver.handle).check()
    }

    override fun close() {
        kraken_clock_destroy(handle).check()
    }
}