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

#ifndef LIBKRAKEN_KRAKEN_BYTE_QUEUE_H
#define LIBKRAKEN_KRAKEN_BYTE_QUEUE_H

#include "kraken_internal.h"

KRAKEN_API_BEGIN

typedef struct kraken_byte_queue* kraken_byte_queue_handle_t;
typedef const struct kraken_byte_queue* kraken_byte_queue_c_handle_t;

kraken_error_t kraken_byte_queue_create(uint32_t size, kraken_byte_queue_handle_t* handle);

kraken_error_t kraken_byte_queue_get_size(kraken_byte_queue_c_handle_t handle, uint32_t* size);

kraken_error_t kraken_byte_queue_is_empty(kraken_byte_queue_c_handle_t handle, bool* result);

kraken_error_t kraken_byte_queue_enqueue(kraken_byte_queue_handle_t handle, uint8_t value);

kraken_error_t kraken_byte_queue_dequeue(kraken_byte_queue_handle_t handle, uint8_t* value);

kraken_error_t kraken_byte_queue_destroy(kraken_byte_queue_handle_t handle);

KRAKEN_API_END

#endif//LIBKRAKEN_KRAKEN_BYTE_QUEUE_H