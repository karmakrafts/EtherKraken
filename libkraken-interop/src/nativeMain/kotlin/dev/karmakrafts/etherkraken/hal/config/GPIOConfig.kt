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
import kotlinx.cinterop.allocArray
import kotlinx.cinterop.cstr
import libkraken.kraken_gpio_config_t
import libkraken.pfn_kraken_gpio_state_init
import libkraken.pfn_kraken_gpio_state_update

data class GPIOConfig(
    val deviceTreeEntry: String,
    val device: String,
    val deviceType: String,
    val registersSize: Long,
    val pins: List<PinConfig>,
    val updateCallback: pfn_kraken_gpio_state_update,
    val initCallback: pfn_kraken_gpio_state_init,
    val alias: String
) {
    internal context(scope: MemScope)
    fun init(config: kraken_gpio_config_t) = with(scope) {
        config.device_tree_entry = deviceTreeEntry.cstr.ptr
        config.device = device.cstr.ptr
        config.device_type = deviceType.cstr.ptr
        config.registers_size = registersSize.toULong()
        config.alias = alias.cstr.ptr
        config.pfn_state_update = updateCallback
        config.pfn_state_init = initCallback
        config.pins = allocArray(pins.size) { pins[it].init(this) }
        config.pin_count = pins.size.toULong()
    }
}

@BoardConfigDsl
class GPIOConfigBuilder @PublishedApi internal constructor(
    private val type: GPIOType
) {
    private lateinit var deviceTreeEntry: String
    private lateinit var device: String
    private var alias: String = "EXT" // The name of the associated port
    private val pins: ArrayList<PinConfig> = ArrayList()

    fun alias(alias: String) {
        this.alias = alias
    }

    fun deviceTreeEntry(entry: String) {
        deviceTreeEntry = entry
    }

    fun device(device: String) {
        this.device = device
    }

    fun pin(devicePin: UInt, portPin: UInt = devicePin) {
        pins += PinConfig(devicePin, portPin)
    }

    @PublishedApi
    internal fun build(): GPIOConfig = GPIOConfig( // @formatter:off
        deviceTreeEntry,
        device,
        type.deviceType,
        type.registersSize,
        pins,
        type.updateCallback,
        type.initCallback,
        alias
    ) // @formatter:on
}

typealias GPIOConfigSpec = GPIOConfigBuilder.() -> Unit

inline fun GPIOConfig(type: GPIOType, spec: GPIOConfigSpec): GPIOConfig = GPIOConfigBuilder(type).apply(spec).build()