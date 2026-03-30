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

import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.MemScope
import kotlinx.cinterop.alloc
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.cstr
import kotlinx.cinterop.ptr
import libkraken.kraken_board_config_t
import libkraken.kraken_gpio_config_t
import libkraken.kraken_mux_config_t

data class BoardConfig( // @formatter:off
    val gpioConfig: GPIOConfig,
    val muxConfigs: List<MuxConfig>,
    val flashDevice: String?
) {
    internal context(scope: MemScope) fun init(config: kraken_board_config_t) = with(scope) {
        config.flash_device = flashDevice?.cstr?.ptr
        config.gpio_config = alloc<kraken_gpio_config_t> { gpioConfig.init(this) }.ptr
        config.mux_configs = allocArray(muxConfigs.size) {
            alloc<kraken_mux_config_t>{ muxConfigs[it].init(this) }.ptr
        }
        config.mux_count = muxConfigs.size.toULong()
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
    internal fun build(): BoardConfig = BoardConfig(gpioConfig, muxConfigs, flashDevice)
}

typealias BoardConfigSpec = BoardConfigBuilder.() -> Unit

inline fun BoardConfig(spec: BoardConfigSpec): BoardConfig = BoardConfigBuilder().apply(spec).build()