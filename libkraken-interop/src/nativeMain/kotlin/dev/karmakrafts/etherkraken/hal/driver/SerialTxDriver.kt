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
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.allocPointerTo
import kotlinx.cinterop.cValue
import kotlinx.cinterop.cValuesOf
import kotlinx.cinterop.convert
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.pointed
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_driver_handle_t
import libkraken.kraken_driver_handle_tVar
import libkraken.kraken_serial_tx_config_t
import libkraken.kraken_serial_tx_driver_create
import libkraken.kraken_serial_tx_driver_destroy
import libkraken.kraken_serial_tx_driver_get_config
import libkraken.kraken_serial_tx_driver_wait
import libkraken.kraken_serial_tx_driver_write

class SerialTxDriver(override val handle: kraken_driver_handle_t) : Driver, AutoCloseable {
    constructor( // @formatter:off
        port: Port,
        dataDevicePin: UInt,
        clockDevicePin: UInt,
        wordSize: Int = Byte.SIZE_BITS,
        bufferSize: Int = 4096
    ) : this(memScoped { // @formatter:on
        val handle = alloc<kraken_driver_handle_tVar>()
        kraken_serial_tx_driver_create(cValue {
            data_pin = dataDevicePin
            clock_pin = clockDevicePin
            word_size = wordSize.toByte()
            buffer_size = bufferSize.toUInt()
        }, port.handle, handle.ptr).check()
        checkNotNull(handle.value) {
            "Could not create serial TX driver"
        }
    })

    inline val wordSize: Int
        get() = memScoped {
            val config = allocPointerTo<kraken_serial_tx_config_t>()
            kraken_serial_tx_driver_get_config(handle, config.ptr).check()
            config.pointed?.word_size?.toInt() ?: 0
        }

    fun wait() {
        kraken_serial_tx_driver_wait(handle).check()
    }

    fun write(value: Byte) {
        val words = Byte.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: Short) {
        val words = Short.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: Int) {
        val words = Int.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: Long) {
        val words = Long.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: UByte) {
        val words = UByte.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: UShort) {
        val words = UShort.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: UInt) {
        val words = UInt.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: ULong) {
        val words = ULong.SIZE_BITS / wordSize
        kraken_serial_tx_driver_write(handle, cValuesOf(value), words.convert()).check()
    }

    fun write(value: String) {
        value.encodeToByteArray().forEach(::write)
    }

    override fun close() {
        kraken_serial_tx_driver_destroy(handle).check()
    }
}