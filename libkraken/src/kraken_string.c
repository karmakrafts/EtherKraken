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

#include "kraken_string.h"
#include "kraken_alloc.h"

#include <stdio.h>

char* kraken_format(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const int length = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    if(length < 0) {
        return nullptr;
    }
    char* memory = kraken_calloc(length + 1);
    va_start(args, fmt);
    if(vsnprintf(memory, length + 1, fmt, args) < 0) {
        va_end(args);
        return nullptr;
    }
    va_end(args);
    return memory;
}

char* kraken_format_v(const char* fmt, const va_list args) {
    const int length = vsnprintf(nullptr, 0, fmt, args);
    if(length < 0) {
        return nullptr;
    }
    char* memory = kraken_calloc(length + 1);
    if(vsnprintf(memory, length + 1, fmt, args) < 0) {
        return nullptr;
    }
    return memory;
}