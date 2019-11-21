#pragma once

namespace fb {

    template <typename It, typename T>
    constexpr It find(It first, It last, const T &value) {
        for (; first != last; ++first)
            if (*first == value)
                break;
        return first;
    }

    template <typename It, typename Pred>
    constexpr It find_if(It first, It last, Pred p) {
        for (; first != last; ++first)
            if (p(*first))
                break;
        return first;
    }

    template <typename It, typename Pred>
    constexpr It find_if_not(It first, It last, Pred q) {
        for (; first != last; ++first)
            if (!q(*first))
                break;
        return first;
    }

}
