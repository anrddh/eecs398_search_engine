#pragma once

namespace fb {

    template <bool B, typename T = void>
    struct EnableIf {};

    template <typename T>
    struct EnableIf<true, T> { using type = T; };

    template <bool B, typename T = void>
    using EnableIfT = typename EnableIf<B,T>::type;

    template <typename T> struct RemoveConst          { using type = T; };
    template <typename T> struct RemoveConst<const T> { using type = T; };
    template <typename T> using RemoveConstT = typename RemoveConst<T>::type;

    template <typename T> struct RemoveVolatile             { using type = T; };
    template <typename T> struct RemoveVolatile<volatile T> { using type = T; };
    template <typename T> using RemoveVolatileT = typename RemoveVolatile<T>::type;

    template <typename T>
    struct RemoveCV {
        using type = RemoveVolatileT<RemoveConstT<T>>;
    };
    template <typename T> using RemoveCVT = typename RemoveCV<T>::type;

};
