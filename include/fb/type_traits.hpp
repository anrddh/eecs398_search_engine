#pragma once

#include "stddef.hpp"

namespace fb {

    /* Helper classes */
    template <typename T, T v>
    struct IntegralConstant {
        static constexpr T value = v;

        using ValueType = T;
        using Type = IntegralConstant;

        constexpr operator ValueType() const noexcept { return value; }
        constexpr ValueType operator()() const noexcept { return value; }
    };

    template <bool B>
    using BoolConstant = IntegralConstant<bool, B>;

    using TrueType = BoolConstant<true>;
    using FalseType = BoolConstant<false>;

    /* Type relationships */
    template <typename T, typename U> struct IsSame : FalseType {};
    template <typename T> struct IsSame<T, T> : TrueType {};
    template <typename T, typename U> inline constexpr bool IsSameV =
        IsSame<T, U>::value;

    /* Const-volatility specifiers */
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

    template <typename T> struct AddConst { using type = const T; };
    template <typename T> using AddConstT = typename AddConst<T>::type;

    template <typename T> struct AddVolatile { using type = volatile T; };
    template <typename T> using AddVolatileT = typename AddVolatile<T>::type;

    template <typename T> struct AddCV { using type = AddConstT<AddVolatileT<T>>; };
    template <typename T> using AddCVT = typename AddCV<T>::type;

    /*  Primary type categories  */
    template <typename T> struct IsArray : FalseType {};
    template <typename T> struct IsArray<T[]> : TrueType {};
    template <typename T, SizeT N> struct IsArray<T[N]> : TrueType {};
    template <typename T> inline constexpr bool IsArrayV = IsArray<T>::value;

    template <typename T> struct IsPointerHelper : FalseType {};
    template <typename T> struct IsPointerHelper<T *> : TrueType {};
    template <typename T> struct IsPointer : IsPointerHelper<RemoveCVT<T>> {};
    template <typename T> inline constexpr bool IsPointerV = IsPointer<T>::value;

    /*  Arrays */
    template <typename T> struct RemoveExtent { using type = T; };
    template <typename T> struct RemoveExtent<T[]> { using type = T; };
    template <typename T, SizeT N>
    struct RemoveExtent<T[N]> { using type = T; };
    template <typename T> using RemoveExtentT = typename RemoveExtent<T>::type;

    /*  Miscellaneous transformations  */
    template <bool B, typename T = void> struct EnableIf {};
    template <typename T> struct EnableIf<true, T> { using type = T; };
    template <bool B, typename T = void> using EnableIfT = typename EnableIf<B,T>::type;

    template <bool B, typename T, typename F> struct Conditional { using type = T; };
    template <typename T, typename F> struct Conditional<false, T, F> { using type = F; };
    template <bool B, typename T, typename F > using ConditionalT = typename Conditional<B,T,F>::type;

    template <typename...> using VoidT = void;

}
