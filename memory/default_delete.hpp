#pragma once

#include <type_traits>

namespace fb {
template <typename T> struct default_delete {
    constexpr default_delete() noexcept = default;

    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    constexpr default_delete(const default_delete<U> &) {}

    constexpr void operator()(T *ptr) const noexcept { delete ptr; }
};

template <typename T> struct default_delete<T[]> {
    constexpr default_delete() noexcept = default;

    template <typename U, typename = std::enable_if_t<
                              std::is_convertible_v<U (*)[], T (*)[]>>>
    constexpr default_delete(const default_delete<U[]> &) {}

    template <typename U>
    constexpr std::enable_if_t<std::is_convertible_v<U *, T *>>
    operator()(U *ptr) const noexcept {
        delete[] ptr;
    }
};
}; // namespace fb
