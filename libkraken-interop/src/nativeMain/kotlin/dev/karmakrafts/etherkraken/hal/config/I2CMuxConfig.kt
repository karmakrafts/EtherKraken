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
import libkraken.kraken_mux_config_t

data class I2CMuxConfig( // @formatter:off
    val bus: String,
    val address: UByte,
    val shadowMemorySize: Long,
    val pins: List<PinConfig>
) : MuxConfig { // @formatter:on
    context(scope: MemScope)
    override fun init(config: kraken_mux_config_t) {
    }
}

@BoardConfigDsl
class I2CMuxConfigBuilder @PublishedApi internal constructor() {
    lateinit var bus: String
    var address: UByte = 0x20U
    var shadowMemorySize: Long = 0
    private val pins: ArrayList<PinConfig> = ArrayList()

    fun pin(devicePin: UInt, portPin: UInt = devicePin) {
        pins += PinConfig(devicePin, portPin)
    }

    @PublishedApi
    internal fun build(): I2CMuxConfig = I2CMuxConfig(bus, address, shadowMemorySize, pins)
}

typealias I2CMuxConfigSpec = I2CMuxConfigBuilder.() -> Unit

inline fun I2CMuxConfig(spec: I2CMuxConfigSpec): I2CMuxConfig = I2CMuxConfigBuilder().apply(spec).build()