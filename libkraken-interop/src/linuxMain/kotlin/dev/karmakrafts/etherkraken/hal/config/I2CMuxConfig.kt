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

import kotlinx.cinterop.ArenaBase
import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.cstr
import kotlinx.cinterop.get
import kotlinx.cinterop.placeTo
import libkraken.kraken_mux_config_t

data class I2CMuxConfig( // @formatter:off
    val bus: String,
    val address: UByte,
    val shadowMemorySize: Long,
    val pins: List<PinConfig>
) : MuxConfig { // @formatter:on
    override fun applyTo(config: kraken_mux_config_t, arena: ArenaBase) = with(config.i2c) {
        this.bus = this@I2CMuxConfig.bus.cstr.placeTo(arena)
        this.address = this@I2CMuxConfig.address
        shadow_memory_size = shadowMemorySize.toULong()
        if (this@I2CMuxConfig.pins.isNotEmpty()) {
            this.pins = arena.allocArray(this@I2CMuxConfig.pins.size)
            pin_count = this@I2CMuxConfig.pins.size.toULong()
            for (index in this@I2CMuxConfig.pins.indices) {
                val pinConfig = this@I2CMuxConfig.pins[index]
                pinConfig.applyTo(pins!![index])
            }
        }
    }
}

@BoardConfigDsl
class I2CMuxConfigBuilder @PublishedApi internal constructor() {
    lateinit var bus: String
    var address: UByte = 0x20U
    var shadowMemorySize: Long = 0
    private val pins: ArrayList<PinConfig> = ArrayList()

    @PublishedApi
    internal fun build(): I2CMuxConfig = I2CMuxConfig(bus, address, shadowMemorySize, pins)
}

typealias I2CMuxConfigSpec = I2CMuxConfigBuilder.() -> Unit

inline fun I2CMuxConfig(spec: I2CMuxConfigSpec): I2CMuxConfig = I2CMuxConfigBuilder().apply(spec).build()