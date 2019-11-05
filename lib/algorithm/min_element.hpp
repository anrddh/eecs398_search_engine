#pragma once

#include "../functional.hpp"
#include "../iterator.hpp"

namespace fb {
    //template <typename It>
    //constexpr It min_element(It begin, It end) {
    //    if (begin == end)
    //        return end;

    //    auto min = begin;

    //    for (; begin != end; ++begin)
    //        if (*begin < *min)
    //            min = begin;

    //    return min;
    //}

    template <typename It,
              typename Compare = Less<typename IteratorTraits<It>::ValueType>>
    constexpr It min_element(It begin, It end, Compare comp) {
        if (begin == end)
            return end;

        auto min = begin;

        for (; begin != end; ++begin)
            if (comp(*begin, *min))
                min = begin;

        return min;
    }

}
