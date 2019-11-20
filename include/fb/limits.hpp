#pragma once

#include "stddef.hpp"

#include <limits.h>

namespace fb {
    template <typename T>
    class NumericLimits {
    public:
        static constexpr T max() noexcept {
            return T();
        }
    };

    template <>
    class NumericLimits<unsigned long> {
    public:
        static constexpr unsigned long max() noexcept {
            return ULONG_MAX;
        }
    };

    template <>
    class NumericLimits<long> {
    public:
        static constexpr long max() noexcept {
            return LONG_MAX;
        }
    };
}
