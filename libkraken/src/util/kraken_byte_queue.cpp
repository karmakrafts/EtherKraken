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

#include "kraken_byte_queue.h"
#include "kraken_byte_queue.hpp"

#include "kraken_api.h"

using namespace kraken;

KRAKEN_API_BEGIN

kraken_error_t kraken_byte_queue_create(const uint32_t size, kraken_byte_queue_handle_t* handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid byte queue handle");
    auto* queue = static_cast<ByteQueue*>(kraken_calloc(sizeof(ByteQueue)));
    new(queue) ByteQueue(size);
    *handle = reinterpret_cast<kraken_byte_queue_handle_t>(queue);
    return KRAKEN_OK;
}

kraken_error_t kraken_byte_queue_get_size(const kraken_byte_queue_c_handle_t handle, uint32_t* size) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid byte queue handle");
    KRAKEN_CHECK_PTR(size, KRAKEN_ERR_INVALID_ARG, "Invalid size pointer");
    const auto* queue = reinterpret_cast<const ByteQueue*>(handle);
    *size = queue->was_size();
    return KRAKEN_OK;
}

kraken_error_t kraken_byte_queue_is_empty(const kraken_byte_queue_c_handle_t handle, bool* result) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid byte queue handle");
    KRAKEN_CHECK_PTR(result, KRAKEN_ERR_INVALID_ARG, "Invalid result pointer");
    const auto* queue = reinterpret_cast<const ByteQueue*>(handle);
    *result = queue->was_empty();
    return KRAKEN_OK;
}

kraken_error_t kraken_byte_queue_enqueue(kraken_byte_queue_handle_t handle, uint8_t value) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid byte queue handle");
    auto* queue = reinterpret_cast<ByteQueue*>(handle);
    return queue->try_push(value) ? KRAKEN_OK : KRAKEN_ERR_INVALID_OP;
}

kraken_error_t kraken_byte_queue_dequeue(kraken_byte_queue_handle_t handle, uint8_t* value) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid byte queue handle");
    KRAKEN_CHECK_PTR(value, KRAKEN_ERR_INVALID_ARG, "Invalid value pointer");
    auto* queue = reinterpret_cast<ByteQueue*>(handle);
    return queue->try_pop(*value) ? KRAKEN_OK : KRAKEN_ERR_INVALID_OP;
}

kraken_error_t kraken_byte_queue_destroy(kraken_byte_queue_handle_t handle) {
    KRAKEN_CHECK_PTR(handle, KRAKEN_ERR_INVALID_ARG, "Invalid byte queue handle");
    auto* queue = reinterpret_cast<ByteQueue*>(handle);
    queue->~ByteQueue();
    kraken_free(queue);
    return KRAKEN_OK;
}

KRAKEN_API_END