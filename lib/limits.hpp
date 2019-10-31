#pragma once

#include "stddef.hpp"

#include <limits.h>

namespace fb {
    template <typename T>
    class numeric_limits {
    public:
        static constexpr T max() noexcept {
            return T();
        }
    };

    template <>
    class numeric_limits<unsigned long> {
    public:
        static constexpr unsigned long max() noexcept {
            return ULONG_MAX;
        }
    };

    template <>
    class numeric_limits<long> {
    public:
        static constexpr long max() noexcept {
            return LONG_MAX;
        }
    };
}
