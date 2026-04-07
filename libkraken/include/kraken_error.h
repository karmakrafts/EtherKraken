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

#ifndef LIBKRAKEN_KRAKEN_ERROR_H
#define LIBKRAKEN_KRAKEN_ERROR_H

#include "kraken_handles.h"

KRAKEN_API_BEGIN

/// @brief Error codes returned by Kraken API functions.
typedef enum kraken_error : int32_t {
    KRAKEN_OK,             ///< No error occurred.
    KRAKEN_EOF,            ///< End of file reached.
    KRAKEN_ERR_INVALID_ARG,///< An invalid argument was passed to the function.
    KRAKEN_ERR_INVALID_OP  ///< The operation is invalid in the current state.
} kraken_error_t;

/// @brief Retrieves the last error message that occurred on the current thread.
/// @return The last error message, or @code nullptr@endcode if no error occurred.
KRAKEN_EXPORT KRAKEN_NODISCARD const char* kraken_last_error_get();

/// @brief Clears the last error message on the current thread.
KRAKEN_EXPORT void kraken_last_error_clear();

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ERROR_H
