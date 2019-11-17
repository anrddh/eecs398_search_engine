#pragma once

namespace fb {
    template <typename InpIt, typename OutIt>
    constexpr OutIt copy(InpIt first, InpIt last, OutIt d) {
        while (first != last)
            *d++ = *first++;
        return d;
    }

    template <typename InpIt, typename OutIt, typename Pred>
    constexpr OutIt copy_if(InpIt first, InpIt last, OutIt d, Pred pred) {
      while (first != last) {
        if (pred(*first))
          *d++ = *first;
        first++;
      }
      return d;
    }
}
