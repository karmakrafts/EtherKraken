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

#ifndef LIBKRAKEN_KRAKEN_INTRIN_H
#define LIBKRAKEN_KRAKEN_INTRIN_H

#include "kraken_api.h"
#include "util/kraken_internal.h"

KRAKEN_API_BEGIN

#ifdef KRAKEN_TEST_ENV
static KRAKEN_INLINE void kraken_io_barrier() {
}
#else
static KRAKEN_INLINE void kraken_io_barrier() {
    asm volatile("dmb ish" ::: "memory");
}
#endif

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_INTRIN_H