#pragma once

#include "../stddef.hpp"
#include "../type_traits.hpp"

#include <type_traits>

namespace fb {

    struct InputIteratorTag {};
    struct OutputIteratorTag {};
    struct ForwardIteratorTag : public InputIteratorTag {};
    struct BidirectionalIteratorTag : public ForwardIteratorTag {};
    struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};
    struct ContiguousIteratorTag : public RandomAccessIteratorTag {};

    template <typename, typename = VoidT<>> struct HasIterTypes : FalseType {};
    template <typename T> struct HasIterTypes<T, VoidT<typename T::IteratorCategory>>
        : TrueType {};

    template <typename Iter, bool>
    class IteratorTraitsHelper {};
    template <typename Iter>
    class IteratorTraitsHelper<Iter, true> {
    public:
        using DifferenceType = typename Iter::DifferenceType;
        using ValueType = typename Iter::ValueType;
        using Pointer = typename Iter::Pointer;
        using Reference = typename Iter::Reference;
        using IteratorCategory = typename Iter::IteratorCategory;
    };

    template <typename Iter>
    class IteratorTraits
        : public IteratorTraitsHelper<Iter, HasIterTypes<Iter>::value> {};

    template <typename T>
    class IteratorTraits<T *> {
    public:
        using DifferenceType = PtrDiffT;
        using ValueType = T;
        using Pointer = T *;
        using Reference = T &;
        using IteratorCategory = RandomAccessIteratorTag;
    };

    template <typename T>
    class IteratorTraits<const T *> {
    public:
        using DifferenceType = PtrDiffT;
        using ValueType = T;
        using Pointer = const T *;
        using Reference = const T &;
        using IteratorCategory = RandomAccessIteratorTag;
    };

}
