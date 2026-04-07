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

#ifndef LIBKRAKEN_KRAKEN_BYTE_QUEUE_HPP
#define LIBKRAKEN_KRAKEN_BYTE_QUEUE_HPP

#include <atomic_queue/atomic_queue.h>
#include <kraken_alloc.h>

namespace kraken {
    template<typename T>
    struct Allocator {
        using value_type = T;

        Allocator() noexcept = default;
        ~Allocator() noexcept = default;

        template<typename U>
        explicit Allocator(const Allocator<U>&) noexcept {
        }

        [[nodiscard]] auto allocate(size_t size) noexcept -> T* {// NOLINT
            return static_cast<T*>(kraken_calloc(sizeof(T) * size));
        }

        auto deallocate(T* address, size_t) noexcept -> void {// NOLINT
            kraken_free(address);
        }
    };

    template<typename T, typename U>
    auto operator==(const Allocator<T>&, const Allocator<U>&) -> bool {
        return true;
    }

    template<typename T, typename U>
    auto operator!=(const Allocator<T>&, const Allocator<U>&) -> bool {
        return false;
    }

    using ByteQueue = atomic_queue::AtomicQueueB2<uint8_t, Allocator<uint8_t>>;
}// namespace kraken

#endif//LIBKRAKEN_KRAKEN_BYTE_QUEUE_HPP