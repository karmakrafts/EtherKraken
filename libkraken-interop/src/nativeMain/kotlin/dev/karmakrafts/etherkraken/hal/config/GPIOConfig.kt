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

package dev.karmakrafts.etherkraken.hal.config

import kotlinx.cinterop.ExperimentalForeignApi
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.get
import kotlinx.cinterop.nativeHeap
import libkraken.kraken_gpio_config_t
import platform.posix.strdup

@OptIn(ExperimentalForeignApi::class)
data class GPIOConfig(
    val deviceTreeEntry: String,
    val device: String,
    val deviceType: String,
    val registersSize: Long,
    val pins: List<PinConfig>
) {
    internal fun applyTo(config: kraken_gpio_config_t) = with(config) {
        device_tree_entry = strdup(deviceTreeEntry)
        device_type = strdup(deviceType)
        this.device = strdup(this@GPIOConfig.device)
        registers_size = registersSize.toULong()
        if (this@GPIOConfig.pins.isNotEmpty()) {
            pins = nativeHeap.allocArray(this@GPIOConfig.pins.size)
            for (index in this@GPIOConfig.pins.indices) {
                val pinConfig = this@GPIOConfig.pins[index]
                pinConfig.applyTo(pins!![index])
            }
        }
    }
}

@BoardConfigDsl
class GPIOConfigBuilder @PublishedApi internal constructor() {
    lateinit var deviceTreeEntry: String
    lateinit var device: String
    lateinit var deviceType: String
    var registersSize: Long = 0L
    private val pins: ArrayList<PinConfig> = ArrayList()

    fun pin(devicePin: UInt, portPin: UInt = devicePin) {
        pins += PinConfig(devicePin, portPin)
    }

    @PublishedApi
    internal fun build(): GPIOConfig = GPIOConfig( // @formatter:off
        deviceTreeEntry,
        device,
        deviceType,
        registersSize,
        pins
    ) // @formatter:on
}

typealias GPIOConfigSpec = GPIOConfigBuilder.() -> Unit

inline fun GPIOConfig(spec: GPIOConfigSpec): GPIOConfig = GPIOConfigBuilder().apply(spec).build()