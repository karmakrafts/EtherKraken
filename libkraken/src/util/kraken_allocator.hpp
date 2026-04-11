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

#ifndef LIBKRAKEN_KRAKEN_ALLOCATOR_HPP
#define LIBKRAKEN_KRAKEN_ALLOCATOR_HPP

#include "kraken_alloc.h"

namespace kraken {
    template<typename T>
    struct Allocator {
        using value_type = T;

        constexpr Allocator() noexcept = default;
        constexpr ~Allocator() noexcept = default;

        template<typename U>
        constexpr Allocator(const Allocator<U>&) noexcept {
        }

        [[nodiscard]] auto allocate(size_t size) noexcept -> T* {// NOLINT
            return static_cast<T*>(kraken_malloc(sizeof(T) * size));
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
}// namespace kraken

#endif//LIBKRAKEN_KRAKEN_ALLOCATOR_HPP