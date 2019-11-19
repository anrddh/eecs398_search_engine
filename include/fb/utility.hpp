#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include "stddef.hpp"
#include <utility>

namespace fb {

    template <typename T, typename V>
    struct Pair {
        T first;
        V second;
    };

    template <typename T1, typename T2>
    [[nodiscard]] Pair<T1,T2> make_pair (T1 &&x, T2 &&y) noexcept {
        return { std::forward<T1>(x), std::forward<T2>(y) };
    }

    // Specialization since we don't want to pass SizeT as ref
    template <typename T>
    [[nodiscard]] Pair<SizeT,T> make_pair (SizeT x, T &&y) noexcept {
        return { x, std::forward<T>(y) };
    }

    template <typename T>
    constexpr void swap(T &a, T &b) noexcept {
        T temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }

} // Namespace fb

#endif
