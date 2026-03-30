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

package dev.karmakrafts.etherkraken.hal

import dev.karmakrafts.etherkraken.hal.config.BoardConfig
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.get
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_board_config_t
import libkraken.kraken_board_create
import libkraken.kraken_board_destroy
import libkraken.kraken_board_get_ports
import libkraken.kraken_board_get_ports_for_type
import libkraken.kraken_board_handle_t
import libkraken.kraken_board_handle_tVar
import libkraken.kraken_port_handle_tVar
import libkraken.kraken_port_type_t
import platform.posix.size_tVar

@OptIn(ExperimentalForeignApi::class)
value class Board private constructor(val handle: kraken_board_handle_t) : AutoCloseable {
    companion object {
        fun create(config: BoardConfig): Board = memScoped {
            val handle = alloc<kraken_board_handle_tVar>()
            kraken_board_create(alloc<kraken_board_config_t> {
                config.init(this)
            }.ptr, handle.ptr).check()
            Board(checkNotNull(handle.value) {
                "Could not create HAL board instance"
            })
        }
    }

    fun enumeratePorts(): List<Port> = memScoped {
        val ports = ArrayList<Port>()
        val count = alloc<size_tVar>()
        kraken_board_get_ports(handle, null, count.ptr).check()
        val portHandles = allocArray<kraken_port_handle_tVar>(count.value.toLong())
        kraken_board_get_ports(handle, portHandles, count.ptr).check()
        for (index in 0UL..<count.value) {
            ports += Port(checkNotNull(portHandles[index.toLong()]) {
                "Could not retrieve port address"
            })
        }
        ports
    }

    fun enumeratePorts(type: kraken_port_type_t): List<Port> = memScoped {
        val ports = ArrayList<Port>()
        val count = alloc<size_tVar>()
        kraken_board_get_ports_for_type(handle, type, null, count.ptr).check()
        val portHandles = allocArray<kraken_port_handle_tVar>(count.value.toLong())
        kraken_board_get_ports_for_type(handle, type, portHandles, count.ptr).check()
        for (index in 0UL..<count.value) {
            ports += Port(checkNotNull(portHandles[index.toLong()]) {
                "Could not retrieve port address"
            })
        }
        ports
    }

    override fun close() {
        kraken_board_destroy(handle)
    }
}