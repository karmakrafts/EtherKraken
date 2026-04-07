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

import dev.karmakrafts.etherkraken.hal.check
import kotlinx.cinterop.ExperimentalForeignApi
import libkraken.kraken_driver_handle_t
import libkraken.kraken_serial_tx_driver_destroy

sealed interface Driver : AutoCloseable {
    val handle: kraken_driver_handle_t
}