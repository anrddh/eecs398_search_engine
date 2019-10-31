#pragma once

#include <limits>

#include "stddef.hpp"

namespace fb {
    inline constexpr SizeT dynamicExtent = std::numeric_limits<SizeT>::max();

    template <typename T, SizeT Extent = dynamicExtent>
    class View {
    public:
        using ElementType = T;
        using IndexType = SizeT;
        using DifferenceType = PtrDiffT;
        using Pointer = T *;
        using ConstPointer = const T *;
        using Reference = T &;
        using ConstReference = const T &;

        static constexpr SizeT extent = Extent;

        /* Constructors */
        constexpr View() noexcept {}

        constexpr View(Pointer ptr_, IndexType count_)
            : ptr(ptr_), count(count_) {}

        constexpr View(Pointer first, Pointer last)
            : ptr(first), count(last - first) {}

        template <SizeT N>
        constexpr View(ElementType (&arr)[N]) noexcept {
        }

        /* Iterators */

        /* Element access */
        [[nodiscard]] constexpr Reference front() const noexcept {
            return *begin();
        }

        [[nodiscard]] constexpr Reference back() const noexcept {
            return *(end() - 1);
        }

        [[nodiscard]] constexpr Reference operator[](IndexType idx) const noexcept {
            return data()[idx];
        }

        [[nodiscard]] constexpr Reference data() const noexcept {
            return ptr;
        }

        /* Observers */
        [[nodiscard]] constexpr bool empty() const noexcept {
            return !size();
        }

        /* Subviews */
        template<SizeT Count >
        constexpr View<ElementType, Count> first() const {
        }

    private:
        Pointer ptr = nullptr;
        SizeT count = extent;
    };

    template <SizeT extent>
    using StringView = View<char, extent>;
}
