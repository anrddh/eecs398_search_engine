#pragma once

namespace fb {

template <typename It, typename T>
constexpr void fill(It first, It last, const T &value) {
    for (; first != last; ++first)
        *first = value;
}

}
