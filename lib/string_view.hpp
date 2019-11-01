#pragma once

#include "view.hpp"
#include "iterator.hpp"
#include "stddef.hpp"
#include "algorithm.hpp"

#include <string.h>

#include <stdexcept>

namespace fb {

    template <typename CharT>
    class BasicStringView {
    public:
        using ValueType = CharT;
        using Pointer = CharT *;
        using ConstPointer = const CharT *;
        using Reference = CharT &;
        using ConstReference = const CharT &;
        using ConstIterator = typename View<CharT,dynamicExtent>::ConstIterator;
        using Iterator = ConstIterator;
        using ConstReverseIterator =
            typename View<CharT,dynamicExtent>::ConstReverseIterator;
        using ReverseIterator = ConstReverseIterator;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;

        static constexpr SizeType npos = SizeType(-1);

        constexpr BasicStringView() noexcept {}
        constexpr BasicStringView(const BasicStringView &) noexcept = default;
        constexpr BasicStringView(const CharT *s, SizeType count)
            : view(s,count) {}
        constexpr BasicStringView(const CharT *s)
            : view(s, strlen(s)) {}
        constexpr BasicStringView & operator=(const BasicStringView &view)
            noexcept = default;

        /* Iterators */
        [[nodiscard]] constexpr Iterator begin() const noexcept {
            return view.begin();
        }

        [[nodiscard]] constexpr ConstIterator cbegin() const noexcept {
            return view.cbegin();
        }

        [[nodiscard]] constexpr Iterator end() const noexcept {
            return view.end();
        }

        [[nodiscard]] constexpr ConstIterator cend() const noexcept {
            return view.cend();
        }

        [[nodiscard]] constexpr ReverseIterator rbegin() const noexcept {
            return view.rbegin();
        }

        [[nodiscard]] constexpr ConstReverseIterator crbegin() const noexcept {
            return view.crbegin();
        }

        [[nodiscard]] constexpr ReverseIterator rend() const noexcept {
            return view.rend();
        }

        [[nodiscard]] constexpr ConstReverseIterator crend() const noexcept {
            return view.crend();
        }

        /* Element access */
        [[nodiscard]] constexpr ConstReference operator[](SizeType pos) const {
            return view[pos];
        }

        [[nodiscard]] constexpr ConstReference at(SizeType pos) const {
            if (pos > size())
                throw std::out_of_range{};

            return view[pos];
        }

        [[nodiscard]] constexpr ConstReference front() const {
            return view.back();
        }

        [[nodiscard]] constexpr ConstReference back() const {
            return view.back();
        }

        [[nodiscard]] constexpr ConstPointer data() const noexcept {
            return view.data();
        }

        /* Capacity  */
        [[nodiscard]] constexpr SizeType size() const noexcept {
            return view.size();
        }

        [[nodiscard]] constexpr SizeType length() const noexcept {
            return size();
        }

        [[nodiscard]] constexpr SizeType max_size() const noexcept {
            return NumericLimits<SizeT>::max() - 1;
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return !size();
        }

        /*  Modifiers */
        constexpr void remove_prefix(SizeType n) {
            view = view.last(size() - n);
        }

        constexpr void remove_suffix(SizeType n) {
            view = view.first(size() - n);
        }

        constexpr void swap(BasicStringView &v) noexcept {
            fb::swap(view, v.view);
        }

        /* Operations */
        constexpr SizeType copy(CharT *dest,
                                SizeType count,
                                SizeType pos = 0) const {
            auto rcount = min(count, ize() - pos);
            fb::copy(data() + pos, data() + pos + rcount, dest);
            return rcount;
        }

        constexpr BasicStringView substr(SizeType pos = 0,
                                         SizeType count = npos) const {
            if (pos > size())
                throw std::out_of_range{};

            return { view.data() + pos, min(count, size() - pos) };
        }

    private:
        View<CharT, dynamicExtent> view;
    };

    using StringView = BasicStringView<char>;

    template <typename CharT>
    constexpr BasicStringView<CharT>::ConstIterator begin(BasicStringView<CharT> sv) noexcept {
        return sv.begin();
    }

    template <typename CharT>
    constexpr BasicStringView<CharT>::ConstIterator end(BasicStringView<CharT> sv) noexcept {
        return sv.end();
    }

    constexpr StringView operator "" sv(const char *str, SizeT len) noexcept {
        return { str, len };
    }
}
