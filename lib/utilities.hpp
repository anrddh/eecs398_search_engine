#ifndef UTILITIES_HPP
#define UTILITIES_HPP

namespace fb {

template <typename T, typename V>
struct Pair {
   T first;
   V second;
};

template <typename T1, typename T2>
Pair<T1,T2> make_pair (T1 x, T2 y) {
    return ( Pair<T1,T2> { x, y } );
};

}; // Namespace fb

#endif
