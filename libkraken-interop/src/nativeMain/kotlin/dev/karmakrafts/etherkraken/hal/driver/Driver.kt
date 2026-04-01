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

package dev.karmakrafts.etherkraken.hal.driver

import dev.karmakrafts.etherkraken.hal.Port
import dev.karmakrafts.etherkraken.hal.check
import kotlinx.cinterop.COpaquePointer
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.StableRef
import kotlinx.cinterop.alloc
import kotlinx.cinterop.asStableRef
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.staticCFunction
import kotlinx.cinterop.value
import libkraken.kraken_driver_create
import libkraken.kraken_driver_destroy
import libkraken.kraken_driver_handle_t
import libkraken.kraken_driver_handle_tVar
import libkraken.kraken_port_handle_t

private fun driverTrampoline(port: kraken_port_handle_t?, userData: COpaquePointer?) {
    val port = Port(port ?: return)
    val driverRef = userData?.asStableRef<Driver>() ?: return
    val driver = driverRef.get()
    val mask = driver.tick()
    port.update(mask)
}

abstract class Driver( // @formatter:off
    port: Port
) : AutoCloseable { // @formatter:on
    private val selfRef: StableRef<Driver> = StableRef.create(this)

    val handle: kraken_driver_handle_t = memScoped {
        val handle = alloc<kraken_driver_handle_tVar>()
        kraken_driver_create(port.handle, staticCFunction(::driverTrampoline), selfRef.asCPointer(), handle.ptr).check()
        checkNotNull(handle.value) { "Could not create driver" }
    }

    abstract fun tick(): ULong

    override fun close() {
        kraken_driver_destroy(handle).check()
        selfRef.dispose()
    }
}