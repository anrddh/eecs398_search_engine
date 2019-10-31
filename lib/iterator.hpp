#pragma once

#include "memory.hpp"
#include "stddef.hpp"

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
        using DifferenceType = PtrDiffT;
        using ValueType = T;
        using Pointer = T *;
        using Reference = T &;
        using IteratorCategory = RandomAccessIteratorTag;
    };

    template <typename T>
    class IteratorTraits<const T *> {
        using DifferenceType = PtrDiffT;
        using ValueType = T;
        using Pointer = const T *;
        using Reference = const T &;
        using IteratorCategory = RandomAccessIteratorTag;
    };

    template <typename Iter>
    class ReverseIterator {
    public:
        using IteratorType = Iter;
        using IteratorCategory = IteratorTraits<Iter>::IteratorCategory;
        using ValueType = IteratorTraits<Iter>::ValueType;
        using DifferenceType = IteratorTraits<Iter>::DifferenceType;
        using Pointer = IteratorTraits<Iter>::Pointer;
        using Reference = IteratorTraits<Iter>::Reference;

        constexpr ReverseIterator() : current() {}
        constexpr explicit ReverseIterator(IteratorType x) : current(x) {}
        template <typename U>
        constexpr ReverseIterator(const ReverseIterator<U> &other)
            : current(other.base()) {}

        template <typename U>
        constexpr ReverseIterator & operator=(const ReverseIterator<U> &other) {
            current = other.base();
            return *this;
        }

        constexpr IteratorType base() const {
            return current;
        }

        constexpr Reference operator*() const {
            return *(--base());
        }

        constexpr Pointer operator->() const {
            return addressof(operator*());
        }

        constexpr Reference operator[](DifferenceType d) const {
            return base()[-d-1];
        }

        constexpr ReverseIterator & operator++() {
            --current;
            return *this;
        }

        constexpr ReverseIterator & operator--() {
            ++current;
            return *this;
        }

        constexpr ReverseIterator operator++(int) {
            auto copy = *this;
            --current;
            return copy;
        }

        constexpr ReverseIterator operator--(int) {
            auto copy = *this;
            ++current;
            return copy;
        }

        constexpr ReverseIterator operator+(DifferenceType n) {
            return ReverseIterator(base() - n);
        }

        constexpr ReverseIterator operator-(DifferenceType n) {
            return ReverseIterator(base() + n);
        }

        constexpr ReverseIterator & operator+=(DifferenceType n) {
            current -= n;
            return *this;
        }

        constexpr ReverseIterator & operator-=(DifferenceType n) {
            current += n;
            return *this;
        }

    protected:
        Iter current;
    };

    template <typename Iter>
    constexpr ReverseIterator<Iter> make_reverse_iterator(Iter i) {
        return ReverseIterator<Iter>(i);
    }

    template <typename Iter1, typename Iter2>
    constexpr bool operator==(const ReverseIterator<Iter1> &lhs,
                              const ReverseIterator<Iter2> &rhs) {
        return lhs.base() == rhs.base();
    }

    template <typename Iter1, typename Iter2>
    constexpr bool operator!=(const ReverseIterator<Iter1> &lhs,
                              const ReverseIterator<Iter2> &rhs) {
        return lhs.base() != rhs.base();
    }

    template <typename Iter1, typename Iter2>
    constexpr bool operator<(const ReverseIterator<Iter1> &lhs,
                             const ReverseIterator<Iter2> &rhs) {
        return lhs.base() > rhs.base();
    }

    template <typename Iter1, typename Iter2>
    constexpr bool operator<=(const ReverseIterator<Iter1> &lhs,
                              const ReverseIterator<Iter2> &rhs) {
        return lhs.base() >= rhs.base();
    }

    template <typename Iter1, typename Iter2>
    constexpr bool operator>(const ReverseIterator<Iter1> &lhs,
                             const ReverseIterator<Iter2> &rhs) {
        return lhs.base() < rhs.base();
    }

    template <typename Iter1, typename Iter2>
    constexpr bool operator>=(const ReverseIterator<Iter1> &lhs,
                              const ReverseIterator<Iter2> &rhs) {
        return lhs.base() <= rhs.base();
    }

    template <typename Iter>
    constexpr ReverseIterator<Iter> operator+(typename ReverseIterator<Iter>::DifferenceType n,
                                              const ReverseIterator<Iter> &it) {
        return ReverseIterator<Iter>(it.base() - n);
    }

    template <typename Iter1, typename Iter2>
    constexpr auto operator-(const ReverseIterator<Iter1> &lhs,
                             const ReverseIterator<Iter2> &rhs)
        -> decltype(rhs.base() - lhs.base()) {
        return rhs.base() - lhs.base();
    }
};
