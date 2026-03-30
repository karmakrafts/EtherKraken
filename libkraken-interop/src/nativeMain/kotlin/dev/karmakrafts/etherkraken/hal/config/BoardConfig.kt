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

package dev.karmakrafts.etherkraken.hal.config

import kotlinx.cinterop.CPointer
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.alloc
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.free
import kotlinx.cinterop.get
import kotlinx.cinterop.nativeHeap
import kotlinx.cinterop.pointed
import kotlinx.cinterop.ptr
import libkraken.kraken_board_config_init
import libkraken.kraken_board_config_t
import libkraken.kraken_board_type_t
import libkraken.kraken_mux_type_t
import platform.posix.free
import platform.posix.strdup

value class BoardConfig @PublishedApi internal constructor(val address: CPointer<kraken_board_config_t>) : AutoCloseable {
    constructor() : this(nativeHeap.alloc<kraken_board_config_t>().ptr)

    constructor(boardType: kraken_board_type_t) : this(nativeHeap.alloc<kraken_board_config_t>().ptr.apply {
        kraken_board_config_init(boardType, this)
    })

    override fun close() {
        address.pointed.gpio_config.apply {
            device_tree_entry?.let(::free)
            device_type?.let(::free)
            device?.let(::free)
            pins?.let(nativeHeap::free)
            alias?.let(::free)
        }
        address.pointed.flash_device?.let(::free)
        address.pointed.mux_configs?.let(nativeHeap::free)
        for (index in 0UL..<address.pointed.mux_count) {
            val muxConfig = address.pointed.mux_configs!![index.toLong()]
            when (muxConfig.type) {
                kraken_mux_type_t.KRAKEN_MUX_TYPE_I2C -> muxConfig.i2c.apply {
                    pins?.let(nativeHeap::free)
                }

                kraken_mux_type_t.KRAKEN_MUX_TYPE_SPI -> muxConfig.spi.apply {
                    pins?.let(nativeHeap::free)
                }
            }
        }
        nativeHeap.free(address)
    }
}

@BoardConfigDsl
class BoardConfigBuilder @PublishedApi internal constructor() {
    @PublishedApi
    internal lateinit var gpioConfig: GPIOConfig

    @PublishedApi
    internal val muxConfigs: ArrayList<MuxConfig> = ArrayList()

    @PublishedApi
    internal var flashDevice: String? = null

    fun flashDevice(device: String) {
        flashDevice = device
    }

    inline fun gpio(type: GPIOType, spec: GPIOConfigSpec) {
        gpioConfig = GPIOConfig(type, spec)
    }

    inline fun i2cMux(spec: I2CMuxConfigSpec) {
        muxConfigs += I2CMuxConfig(spec)
    }

    inline fun spiMux(spec: SPIMuxConfigSpec) {
        muxConfigs += SPIMuxConfig(spec)
    }

    @PublishedApi
    internal fun build(): BoardConfig = BoardConfig().apply {
        gpioConfig.applyTo(address.pointed.gpio_config)
        flashDevice?.let { flashDevice ->
            address.pointed.flash_device = strdup(flashDevice)
        }
        if (muxConfigs.isNotEmpty()) {
            address.pointed.mux_configs = nativeHeap.allocArray(muxConfigs.size)
            address.pointed.mux_count = muxConfigs.size.toULong()
            for (index in muxConfigs.indices) {
                val targetConfig = address.pointed.mux_configs!![index]
                muxConfigs[index].applyTo(targetConfig)
            }
        }
    }
}

typealias BoardConfigSpec = BoardConfigBuilder.() -> Unit

inline fun BoardConfig(spec: BoardConfigSpec): BoardConfig = BoardConfigBuilder().apply(spec).build()