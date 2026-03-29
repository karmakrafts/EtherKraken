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

data class SPIMuxConfig( // @formatter:off
    val device: String,
    val pins: List<PinConfig>
) : MuxConfig { // @formatter:on
    override fun applyTo(config: kraken_mux_config_t, arena: ArenaBase) = with(config.spi) {
        this.device = this@SPIMuxConfig.device.cstr.placeTo(arena)
        if (this@SPIMuxConfig.pins.isNotEmpty()) {
            this.pins = arena.allocArray(this@SPIMuxConfig.pins.size)
            pin_count = this@SPIMuxConfig.pins.size.toULong()
            for (index in this@SPIMuxConfig.pins.indices) {
                val pinConfig = this@SPIMuxConfig.pins[index]
                pinConfig.applyTo(pins!![index])
            }
        }
    }
}

@BoardConfigDsl
class SPIMuxConfigBuilder @PublishedApi internal constructor() {
    lateinit var device: String
    private val pins: ArrayList<PinConfig> = ArrayList()

    @PublishedApi
    internal fun build(): SPIMuxConfig = SPIMuxConfig(device, pins)
}

typealias SPIMuxConfigSpec = SPIMuxConfigBuilder.() -> Unit

inline fun SPIMuxConfig(spec: SPIMuxConfigSpec): SPIMuxConfig = SPIMuxConfigBuilder().apply(spec).build()