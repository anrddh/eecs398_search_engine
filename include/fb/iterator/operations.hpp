#pragma once

#include "../type_traits.hpp"
#include "traits.hpp"

namespace fb {

    template <typename It>
    constexpr typename IteratorTraits<It>::DifferenceType
    distance(It first, It last, RandomAccessIteratorTag) {
        return last - first;
    }

    template <typename It>
    constexpr typename IteratorTraits<It>::DifferenceType
    distance(It first, It last, InputIteratorTag) {
        typename IteratorTraits<It>::DifferenceType diff = 0;
        for ( ; first != last; ++first)
            ++diff;
        return diff;
    }

    template <typename It>
    constexpr typename IteratorTraits<It>::DifferenceType
    distance(It first, It last) {
        return distance(first, last,
                        typename IteratorTraits<It>::IteratorCategory());
    }

}
