// Copyright 2026 Karma Krafts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LIBKRAKEN_KRAKEN_LOG_H
#define LIBKRAKEN_KRAKEN_LOG_H

#include "kraken_api.h"
#include "kraken_error.h"

KRAKEN_API_BEGIN

/// @brief Logging levels.
typedef enum kraken_log_level : uint8_t {
    KRAKEN_LOG_LEVEL_DEBUG,///< Debug log level.
    KRAKEN_LOG_LEVEL_INFO, ///< Informational log level.
    KRAKEN_LOG_LEVEL_WARN, ///< Warning log level.
    KRAKEN_LOG_LEVEL_ERROR,///< Error log level.
} kraken_log_level_t;

/// @brief Callback function type for log consumers.
/// @param[in] level The log level of the message.
/// @param[in] message The log message.
typedef void (*pfn_kraken_log_consumer)(kraken_log_level_t level, const char* message);

/// @brief Sets the log consumer for the library.
/// @param[in] consumer The log consumer callback function.
/// @return KRAKEN_SUCCESS on success, or an error code on failure.
KRAKEN_EXPORT kraken_error_t kraken_log_set_consumer(pfn_kraken_log_consumer consumer);

/// @brief Gets the current log consumer for the library.
/// @param[out] consumer Pointer to store the current log consumer callback function.
/// @return KRAKEN_SUCCESS on success, or an error code on failure.
KRAKEN_EXPORT kraken_error_t kraken_log_get_consumer(pfn_kraken_log_consumer* consumer);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_LOG_H
