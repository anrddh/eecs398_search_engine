#pragma once

namespace fb {

template <typename T>
T * addressof(T &arg) noexcept {
    return reinterpret_cast<T *>(&const_cast<char &>(reinterpret_cast<const volatile char &>(arg)));
}

template <typename T>
const T * addressof(const T &&) = delete;

}
