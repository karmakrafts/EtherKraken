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

import kotlinx.cinterop.ByteVar
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.get
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.toKStringFromUtf8
import kotlinx.cinterop.value
import libkraken.kraken_io_handle_tVar
import libkraken.kraken_port_get_ios
import libkraken.kraken_port_get_name
import libkraken.kraken_port_get_type
import libkraken.kraken_port_handle_t
import libkraken.kraken_port_type_t
import libkraken.kraken_port_type_tVar
import libkraken.kraken_port_update
import platform.posix.size_tVar

value class Port(val handle: kraken_port_handle_t) {
    inline val type: kraken_port_type_t
        get() = memScoped {
            val type = alloc<kraken_port_type_tVar>()
            kraken_port_get_type(handle, type.ptr).check()
            type.value
        }

    inline val name: String
        get() = memScoped {
            val size = alloc<size_tVar>()
            kraken_port_get_name(handle, null, size.ptr).check()
            val buffer = allocArray<ByteVar>(size.value.toLong())
            kraken_port_get_name(handle, buffer, size.ptr).check()
            buffer.toKStringFromUtf8()
        }

    fun update() {
        kraken_port_update(handle).check()
    }

    fun enumerateIOs(): Sequence<IO> = sequence {
        memScoped {
            val count = alloc<size_tVar>()
            kraken_port_get_ios(handle, null, count.ptr).check()
            val ios = allocArray<kraken_io_handle_tVar>(count.value.toLong())
            kraken_port_get_ios(handle, ios, count.ptr).check()
            for (index in 0UL..<count.value) {
                yield(IO(checkNotNull(ios[index.toLong()]) {
                    "Could not retrieve IO address"
                }))
            }
        }
    }
}