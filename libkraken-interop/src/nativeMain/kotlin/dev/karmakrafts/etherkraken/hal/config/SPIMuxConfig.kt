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

data class SPIMuxConfig( // @formatter:off
    val device: String,
    val pins: List<PinConfig>
) : MuxConfig { // @formatter:on
    context(scope: MemScope)
    override fun init(config: kraken_mux_config_t) {
    }
}

@BoardConfigDsl
class SPIMuxConfigBuilder @PublishedApi internal constructor() {
    lateinit var device: String
    private val pins: ArrayList<PinConfig> = ArrayList()

    fun pin(devicePin: UInt, portPin: UInt = devicePin) {
        pins += PinConfig(devicePin, portPin)
    }

    @PublishedApi
    internal fun build(): SPIMuxConfig = SPIMuxConfig(device, pins)
}

typealias SPIMuxConfigSpec = SPIMuxConfigBuilder.() -> Unit

inline fun SPIMuxConfig(spec: SPIMuxConfigSpec): SPIMuxConfig = SPIMuxConfigBuilder().apply(spec).build()