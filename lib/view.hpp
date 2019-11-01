#pragma once

#include "limits.hpp"
#include "stddef.hpp"
#include "type_traits.hpp"
#include "iterator.hpp"
#include "utility.hpp"

namespace fb {
    template <typename T, SizeT>
    class Array;

    constexpr SizeT dynamicExtent = NumericLimits<SizeT>::max();

    template <typename T, SizeT Extent = dynamicExtent>
    class View {
    public:
        using ElementType = T;
        using ValueType = RemoveCVT<T>;
        using IndexType = SizeT;
        using DifferenceType = PtrDiffT;
        using Pointer = T *;
        using ConstPointer = const T *;
        using Reference = T &;
        using ConstReference = const T &;
        using Iterator = Pointer;
        using ConstIterator = ConstPointer;
        using ReverseIterator = ReverseIterator<Iterator>;
        using ConstReverseIterator = fb::ReverseIterator<ConstIterator>;

        static constexpr SizeT extent = Extent;

        /* Constructors */
        template <typename D = void,
                  typename = EnableIfT<extent == 0 || extent == dynamicExtent, D>>
        constexpr View() noexcept {}

        constexpr View(Pointer ptr_, IndexType count_)
            : ptr(ptr_), count(count_) {}

        constexpr View(Pointer first, Pointer last)
            : View(first, last - first) {}

        template <SizeT N,
                  typename = EnableIfT<extent == N || extent == dynamicExtent>>
        constexpr View(ElementType (&arr)[N]) noexcept
            : View(data(arr),N) {}

        template <SizeT N,
                  typename = EnableIfT<extent == N || extent == dynamicExtent>>
        constexpr View(Array<ValueType, N> &arr) noexcept
            : View(data(arr),N) {}

        template <SizeT N,
                  typename = EnableIfT<extent == N || extent == dynamicExtent>>
        constexpr View(const Array<ValueType, N> &arr) noexcept
            : View(data(arr),N) {}

        template <typename Container>
        constexpr View(Container &cont)
            : View(data(cont), size(cont)) {}

        template <typename Container>
        constexpr View(const Container &cont)
            : View(data(cont), size(cont)) {}

        template <typename U, SizeT N>
        constexpr View(const View<U, N> &s) noexcept
            : View(s.data(), s.size()) {}

        constexpr View(const View &other) noexcept = default;
        constexpr View & operator=(const View &other) noexcept = default;

        /* Iterators */
        [[nodiscard]] constexpr Iterator begin() const noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr ConstIterator cbegin() const noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr Iterator end() const noexcept {
            return ptr + size();
        }

        [[nodiscard]] constexpr ConstIterator cend() const noexcept {
            return ptr + size();
        }

        [[nodiscard]] constexpr ReverseIterator rbegin() const noexcept {
            return end() - 1;
        }

        [[nodiscard]] constexpr ConstReverseIterator crbegin() const noexcept {
            return cend() - 1;
        }

        [[nodiscard]] constexpr ReverseIterator rend() const noexcept {
            return begin() - 1;
        }

        [[nodiscard]] constexpr ConstReverseIterator crend() const noexcept {
            return cbegin() - 1;
        }

        /* Element access */
        [[nodiscard]] constexpr Reference front() const noexcept {
            return *cbegin();
        }

        [[nodiscard]] constexpr Reference back() const noexcept {
            return *(cend() - 1);
        }

        [[nodiscard]] constexpr Reference operator[](IndexType idx) const noexcept {
            return data()[idx];
        }

        [[nodiscard]] constexpr Pointer data() const noexcept {
            return ptr;
        }

        /* Observers */
        [[nodiscard]] constexpr IndexType size() const noexcept {
            return count;
        }

        [[nodiscard]] constexpr IndexType size_bytes() const noexcept {
            return size() * sizeof(ElementType);
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return !size();
        }

        /* Subviews */
        template <SizeT Count>
        constexpr View<ElementType, Count> first() const {
            return { data(), Count };
        }
        constexpr View<ElementType, dynamicExtent> first(SizeT n) const {
            return { data(), n };
        }

        template <SizeT Count>
        constexpr View<ElementType, Count> last() const {
            return { data() + size() - Count, Count };
        }
        constexpr View<ElementType, dynamicExtent> last(SizeT n) const {
            return { data() + size() - n, n };
        }

        template <SizeT Offset, SizeT Count = dynamicExtent>
        constexpr auto subview() const {
            if constexpr (Count != dynamicExtent) {
                return View<ElementType, Count>(data() + Offset, Count);
            }

            if constexpr (Extent != dynamicExtent) {
                return View<ElementType, Extent - Offset>(data() + Offset,
                                                          Extent - Offset);
            }

            return View<ElementType, dynamicExtent>(data() + Offset, Count);
        }

        constexpr View<ElementType, dynamicExtent> subview(SizeT Offset,
                                                           SizeT Count = dynamicExtent) const {
            return { data() + Offset, Count };
        }

        constexpr void swap(const View &other) {
            fb::swap(ptr, other.ptr);
            fb::swap(count, other.count);
        }

    private:
        Pointer ptr = nullptr;
        SizeT count = extent;
    };

    template <typename T, SizeT N>
    constexpr View<T,N>::Iterator begin(View<T,N> s) noexcept {
        return s.begin();
    }

    template <typename T, SizeT N>
    constexpr View::Iterator end(View<T,N> s) noexcept {
        return s.end();
    }

    template <SizeT I, typename T, SizeT N>
    constexpr T & get(View<T,N> s) noexcept {
        static_assert(N != dynamicExtent, "");
        static_assert(0 <= I && I < N, "");

        return s[I];
    }

    template <typename T, SizeT N>
    void swap(View<T,N> &v1, View<T,N> &v2) {
        v1.swap(v2);
    }
}
