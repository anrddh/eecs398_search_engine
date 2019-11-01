#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <utility>

namespace fb {

    template <typename T, typename V>
    struct Pair {
        T first;
        V second;
    };

   template <typename T>
   constexpr void swap(T &a, T &b) noexcept {
       T temp = std::move(a);
       a = std::move(b);
       b = std::move(temp);
   }

}; // Namespace fb

#endif
