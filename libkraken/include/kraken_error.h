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

typedef enum kraken_error : int32_t {
    KRAKEN_OK,
    KRAKEN_EOF,
    KRAKEN_ERR_INVALID_ARG,
    KRAKEN_ERR_INVALID_OP
} kraken_error_t;

KRAKEN_EXPORT const char* kraken_last_error_get();
KRAKEN_EXPORT void kraken_last_error_clear();

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_ERROR_H
