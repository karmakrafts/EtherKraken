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
import kotlinx.cinterop.sizeOf
import kotlinx.cinterop.staticCFunction
import libkraken.bcm2835_gpio_state_init
import libkraken.bcm2835_gpio_state_update
import libkraken.bcm2835_gpio_t
import libkraken.pfn_kraken_gpio_state_init
import libkraken.pfn_kraken_gpio_state_update

enum class DefaultGPIOType( // @formatter:off
    override val deviceType: String,
    override val registersSize: Long,
    override val updateCallback: pfn_kraken_gpio_state_update,
    override val initCallback: pfn_kraken_gpio_state_init,
) : GPIOType { // @formatter:on
    BCM2835("bcm2835", sizeOf<bcm2835_gpio_t>(), staticCFunction { baseAddress, shadowMemory, ios, ioCount ->
        bcm2835_gpio_state_update(baseAddress, shadowMemory, ios, ioCount)
    }, staticCFunction { baseAddress, shadowMemory, ios, ioCount ->
        bcm2835_gpio_state_init(baseAddress, shadowMemory, ios, ioCount)
    })
}