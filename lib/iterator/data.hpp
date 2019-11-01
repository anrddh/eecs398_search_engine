#pragma once

#include "../stddef.hpp"

#include <initializer_list>

namespace fb {
    template <typename C>
    constexpr auto data(C &c) -> decltype(c.data()) {
        return c.data();
    }

    template <typename C>
    constexpr auto data(const C &c) -> decltype(c.data()) {
        return c.data();
    }

    template <typename T, SizeT N>
    constexpr T * data(T (&array)[N]) noexcept {
        return array;
    }

    template <typename E>
    constexpr const E * data(std::initializer_list<E> il) noexcept {
        returnn il.begin();
    }
};
