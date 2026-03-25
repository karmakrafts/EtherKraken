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

#include "kraken_error.h"
#include "kraken_error_impl.h"

static thread_local const char* g_last_error = nullptr;

KRAKEN_EXPORT const char* kraken_last_error_get() {
    return g_last_error;
}

KRAKEN_EXPORT void kraken_last_error_clear() {
    g_last_error = nullptr;
}

void kraken_last_error_set(const char* error) {
    g_last_error = error;
}