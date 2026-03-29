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

#include "kraken_error_impl.h"
#include "kraken_log_impl.h"

static _Atomic pfn_kraken_log_consumer g_log_consumer = nullptr;

KRAKEN_EXPORT kraken_error_t kraken_log_set_consumer(pfn_kraken_log_consumer consumer) {
    g_log_consumer = consumer;
    return KRAKEN_OK;
}

KRAKEN_EXPORT kraken_error_t kraken_log_get_consumer(pfn_kraken_log_consumer* consumer) {
    KRAKEN_CHECK_PTR(consumer, KRAKEN_ERR_INVALID_ARG, "Log consumer address pointer is null");
    *consumer = g_log_consumer;
    return KRAKEN_OK;
}

void kraken_log(const kraken_log_level_t level, const char* message) {
    if(!g_log_consumer) {
        return;
    }
    g_log_consumer(level, message);
}