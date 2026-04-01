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

import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_dispatcher_create
import libkraken.kraken_dispatcher_destroy
import libkraken.kraken_dispatcher_handle_t
import libkraken.kraken_dispatcher_handle_tVar
import libkraken.kraken_dispatcher_register
import libkraken.kraken_dispatcher_unregister

value class Dispatcher private constructor(val handle: kraken_dispatcher_handle_t) : AutoCloseable {
    constructor(coreAffinity: Int) : this(memScoped {
        val handle = alloc<kraken_dispatcher_handle_tVar>()
        kraken_dispatcher_create(coreAffinity, handle.ptr).check()
        checkNotNull(handle.value) { "Could not create dispatcher" }
    })

    operator fun plusAssign(clock: Clock) {
        kraken_dispatcher_register(handle, clock.handle).check()
    }

    operator fun minusAssign(clock: Clock) {
        kraken_dispatcher_unregister(handle, clock.handle).check()
    }

    override fun close() {
        kraken_dispatcher_destroy(handle).check()
    }
}