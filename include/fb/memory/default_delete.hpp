#pragma once

#include <type_traits>

#include "../type_traits.hpp"

namespace fb {
template <typename T> struct DefaultDelete {
    constexpr DefaultDelete() noexcept = default;

    template <typename U,
              typename = EnableIfT<std::is_convertible_v<U *, T *>>>
    constexpr DefaultDelete(const DefaultDelete<U> &) {}

    constexpr void operator()(T *ptr) const noexcept { delete ptr; }
};

template <typename T> struct DefaultDelete<T[]> {
    constexpr DefaultDelete() noexcept = default;

    template <typename U, typename = EnableIfT<
                              std::is_convertible_v<U (*)[], T (*)[]>>>
    constexpr DefaultDelete(const DefaultDelete<U[]> &) {}

    template <typename U>
    constexpr EnableIfT<std::is_convertible_v<U *, T *>>
    operator()(U *ptr) const noexcept {
        delete[] ptr;
    }
};
} // namespace fb
