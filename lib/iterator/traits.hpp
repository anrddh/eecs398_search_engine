#pragma once

#include "../stddef.hpp"

namespace fb {

    struct InputIteratorTag {};
    struct OutputIteratorTag {};
    struct ForwardIteratorTag : public InputIteratorTag {};
    struct BidirectionalIteratorTag : public ForwardIteratorTag {};
    struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};
    struct ContiguousIteratorTag : public RandomAccessIteratorTag {};

    template <typename Iter>
    class IteratorTraits {
    public:
        using DifferenceType = typename Iter::DifferenceType;
        using ValueType = typename Iter::ValueType;
        using Pointer = typename Iter::Pointer;
        using Reference = typename Iter::Reference;
        using IteratorCategory = typename Iter::IteratorCategory;
    };

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
