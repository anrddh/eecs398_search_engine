#pragma once

#include "min_element.hpp"
#include "../functional.hpp"
#include "../iterator.hpp"

#include <initializer_list>

namespace fb {

    template <typename T, typename Compare = Less<T>>
    constexpr const T & min(const T &a, const T &b, Compare comp = Compare()) {
        return comp(a,b) ? a : b;
    }

    template <typename T, typename Compare = Less<T>>
    constexpr T min(std::initializer_list<T> ilist, Compare comp = Compare()) {
        return *min_element(ilist.begin(), ilist.end(), comp);
    }

}
