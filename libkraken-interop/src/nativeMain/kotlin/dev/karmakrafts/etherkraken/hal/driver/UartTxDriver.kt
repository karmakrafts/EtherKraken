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
import kotlinx.cinterop.cValue
import kotlinx.cinterop.cValuesOf
import kotlinx.cinterop.convert
import kotlinx.cinterop.memScoped
import kotlinx.cinterop.ptr
import kotlinx.cinterop.value
import libkraken.kraken_driver_handle_t
import libkraken.kraken_driver_handle_tVar
import libkraken.kraken_uart_parity_t
import libkraken.kraken_uart_tx_driver_create
import libkraken.kraken_uart_tx_driver_destroy
import libkraken.kraken_uart_tx_driver_wait
import libkraken.kraken_uart_tx_driver_write

class UartTxDriver(override val handle: kraken_driver_handle_t) : Driver, AutoCloseable {
    constructor( // @formatter:off
        port: Port,
        pin: UInt,
        dataBits: Int = 8,
        stopBits: Int = 1,
        parity: kraken_uart_parity_t = kraken_uart_parity_t.KRAKEN_UART_PARITY_NONE,
        bufferSize: Int = 4096
    ) : this(memScoped { // @formatter:on
        val handle = alloc<kraken_driver_handle_tVar>()
        kraken_uart_tx_driver_create(cValue {
            buffer_size = bufferSize.toUInt()
            this.pin = pin
            data_bits = dataBits.toByte()
            stop_bits = stopBits.toByte()
            this.parity = parity
        }, port.handle, handle.ptr).check()
        checkNotNull(handle.value) {
            "Could not create serial TX driver"
        }
    })

    fun wait() {
        kraken_uart_tx_driver_wait(handle).check()
    }

    fun write(value: Byte) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), Byte.SIZE_BYTES.convert()).check()
    }

    fun write(value: Short) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), Short.SIZE_BYTES.convert()).check()
    }

    fun write(value: Int) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), Int.SIZE_BYTES.convert()).check()
    }

    fun write(value: Long) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), Long.SIZE_BYTES.convert()).check()
    }

    fun write(value: UByte) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), UByte.SIZE_BYTES.convert()).check()
    }

    fun write(value: UShort) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), UShort.SIZE_BYTES.convert()).check()
    }

    fun write(value: UInt) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), UInt.SIZE_BYTES.convert()).check()
    }

    fun write(value: ULong) {
        kraken_uart_tx_driver_write(handle, cValuesOf(value), ULong.SIZE_BYTES.convert()).check()
    }

    fun write(value: String) {
        value.encodeToByteArray().forEach(::write)
    }

    override fun close() {
        kraken_uart_tx_driver_destroy(handle).check()
    }
}