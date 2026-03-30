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
import kotlinx.cinterop.allocPointerTo
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.toKStringFromUtf8
import kotlinx.cinterop.value
import libkraken.kraken_bool_t
import libkraken.kraken_bool_tVar
import libkraken.kraken_io_get
import libkraken.kraken_io_get_mode
import libkraken.kraken_io_get_name
import libkraken.kraken_io_handle_t
import libkraken.kraken_io_mode_t
import libkraken.kraken_io_mode_tVar
import libkraken.kraken_io_set
import libkraken.kraken_io_set_mode

value class IO(val handle: kraken_io_handle_t) {
    inline var mode: kraken_io_mode_t
        get() = memScoped {
            val mode = alloc<kraken_io_mode_tVar>()
            kraken_io_get_mode(handle, mode.ptr).check()
            mode.value
        }
        set(value) = memScoped {
            kraken_io_set_mode(handle, value).check()
        }

    inline var state: Boolean
        get() = memScoped {
            val state = alloc<kraken_bool_tVar>()
            kraken_io_get(handle, state.ptr).check()
            state.value == kraken_bool_t.KRAKEN_TRUE
        }
        set(value) = memScoped {
            kraken_io_set(handle, if (value) kraken_bool_t.KRAKEN_TRUE else kraken_bool_t.KRAKEN_FALSE).check()
        }

    inline val name: String
        get() = memScoped {
            val name = allocPointerTo<ByteVar>()
            kraken_io_get_name(handle, name.ptr).check()
            name.value?.toKStringFromUtf8() ?: "Unknown"
        }
}