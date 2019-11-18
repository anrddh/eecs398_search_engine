#pragma once

#include "view.hpp"
#include "iterator.hpp"
#include "stddef.hpp"
#include "algorithm.hpp"
#include "memory.hpp"

#include <stdexcept>
#include <iostream>

#include <string.h>

namespace fb {

    template <typename CharT>
    class BasicStringView {
    public:
        using ValueType = CharT;
        using Pointer = CharT *;
        using ConstPointer = const CharT *;
        using Reference = CharT &;
        using ConstReference = const CharT &;
        using ConstIterator = ConstPointer;
        using Iterator = ConstIterator;
        using ConstReverseIterator = fb::ReverseIterator<ConstIterator>;
        using ReverseIterator = ConstReverseIterator;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;

        static constexpr SizeType npos = SizeType(-1);

        constexpr BasicStringView() noexcept {}
        constexpr BasicStringView(const BasicStringView &) noexcept = default;
        constexpr BasicStringView(const CharT *s, SizeType count)
            : ptr(s), len(count) {}
        constexpr BasicStringView(const CharT *s)
            : BasicStringView(s, strlen(s)) {}
        constexpr BasicStringView & operator=(const BasicStringView &view)
            noexcept = default;

        /* Iterators */
        [[nodiscard]] constexpr Iterator begin() const noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr ConstIterator cbegin() const noexcept {
            return ptr;
        }

        [[nodiscard]] constexpr Iterator end() const noexcept {
            return ptr + len;
        }

        [[nodiscard]] constexpr ConstIterator cend() const noexcept {
            return ptr + len;
        }

        [[nodiscard]] constexpr ReverseIterator rbegin() const noexcept {
            return ReverseIterator(end());
        }

        [[nodiscard]] constexpr ConstReverseIterator crbegin() const noexcept {
            return ConstReverseIterator(cend());
        }

        [[nodiscard]] constexpr ReverseIterator rend() const noexcept {
            return ReverseIterator(begin());
        }

        [[nodiscard]] constexpr ConstReverseIterator crend() const noexcept {
            return ConstReverseIterator(cbegin());
        }

        /* Element access */
        [[nodiscard]] constexpr ConstReference operator[](SizeType pos)
            const noexcept {
            return data()[pos];
        }

        [[nodiscard]] constexpr ConstReference at(SizeType pos) const {
            if (pos > size())
                throw std::out_of_range("");

            return data()[pos];
        }

        [[nodiscard]] constexpr ConstReference front() const {
            return data()[0];
        }

        [[nodiscard]] constexpr ConstReference back() const {
            return data()[size() - 1];
        }

        [[nodiscard]] constexpr ConstPointer data() const noexcept {
            return ptr;
        }

        /* Capacity  */
        [[nodiscard]] constexpr SizeType size() const noexcept {
            return len;
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
        constexpr void removePrefix(SizeType n) {
            if (n > len) {
                ptr = nullptr;
                len = 0;
                return;
            }

            ptr += n;
            len -= n;
        }

        constexpr void removeSuffix(SizeType n) {
            len -= n;
        }

        constexpr void swap(BasicStringView &v) noexcept {
            fb::swap(ptr, v.ptr);
            fb::swap(len, v.len);
        }

        /* Operations */
        constexpr SizeType copy(CharT *dest,
                                SizeType count,
                                SizeType pos = 0) const {
            auto rcount = min(count, size() - pos);
            fb::copy(data() + pos, data() + pos + rcount, dest);
            return rcount;
        }

        constexpr BasicStringView substr(SizeType pos = 0,
                                         SizeType count = npos) const {
            if (pos > size())
                throw std::out_of_range("");

            return { data() + pos, fb::min(count, size()) };
        }

        constexpr int compare(BasicStringView v) const noexcept {
            auto cmp = strncmp(data(), v.data(), fb::min(size(), v.size()));
            return cmp ? cmp : size() - v.size();
        }

        constexpr int compare(SizeType pos1, SizeType count1,
                              BasicStringView v) const {
            return substr(pos1, count1).compare(v);
        }

        constexpr int compare(SizeType pos1, SizeType count1,
                              BasicStringView  v,
                              SizeType pos2, SizeType count2) const {
            return substr(pos1, count1).compare(v.substr(pos2, count2));
        }

        constexpr int compare(const CharT *s) const {
            auto ptr = data();
            auto len = size();
            auto begin = s;

            for ( ; len && *s; --len, ++ptr, ++s)
                if (*s != *ptr)
                    return *ptr - *s;

            return size() - (s - begin);
        }

        constexpr int compare(SizeType pos1, SizeType count1,
                              const CharT *s) const {
            return substr(pos1, count1).compare(s);
        }

        constexpr int compare(SizeType pos1, SizeType count1,
                              const CharT *s, SizeType count2) const {
            return strncmp(data() + pos1, s, fb::min(count1, count2));
        }

        constexpr bool startsWith(BasicStringView x) const noexcept {
            return size() >= x.size() && !compare(0, x.size(), x);
        }

        constexpr bool startsWith(CharT x) const noexcept {
            return !empty() && front() == x;
        }

        constexpr bool startsWith(const CharT *x) const {
            for (auto c : *this) {
                if (!*x)
                    return true;
                if (c != *x)
                    return false;
                ++x;
            }

            if (*x)
                return false;
            return true;
        }

        constexpr bool endsWith(BasicStringView x) const noexcept {
            return size() >= x.size() &&
                compare(size() - x.size(), npos, x) == 0;
        }

        constexpr bool endsWith(CharT x) const noexcept {
            return !empty() && back() == x;
        }

        constexpr bool endsWith(const CharT *x) const {
            return endsWith(BasicStringView(x));
        }

        constexpr SizeType find(BasicStringView v, SizeType pos = 0) const noexcept {
            if (v.empty())
                return 0;

            auto len = size() - pos;
            auto ptr = data() + pos;

            for ( ; v.size() <= len; --len, ++ptr)
                if (!memcmp(ptr, v.data(), v.size()))
                    return ptr - data();

            return npos;
        }

        constexpr SizeType find(CharT ch, SizeType pos = 0) const noexcept {
            return find(fb::addressof(ch), pos, 1);
        }

        constexpr SizeType find(const CharT *s, SizeType pos, SizeType count) const {
            return find(BasicStringView(s, count), pos);
        }

        constexpr SizeType find(const CharT *s, SizeType pos = 0) const {
            return strnstr(data() + pos + 1, s, size() - pos - 1);
        }

    private:
        ConstPointer ptr = nullptr;
        SizeT len = 0;
    };

    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& os,
               fb::BasicStringView<CharT> v) {
        for (auto c : v)
            os << c;
        return os;
    }

    template <typename CharT>
    constexpr bool operator==(BasicStringView<CharT> lhs,
                              BasicStringView<CharT> rhs) noexcept {
        return !lhs.compare(rhs);
    }

    template <typename CharT>
    constexpr bool operator!=(BasicStringView<CharT> lhs,
                              BasicStringView<CharT> rhs) noexcept {
        return !(lhs == rhs);
    }

    template <typename CharT>
    constexpr bool operator<(BasicStringView<CharT> lhs,
                             BasicStringView<CharT> rhs) noexcept {
        return lhs.compare(rhs) < 0;
    }

    template <typename CharT>
    constexpr bool operator<=(BasicStringView<CharT> lhs,
                              BasicStringView<CharT> rhs) noexcept {
        return !(lhs > rhs);
    }

    template <typename CharT>
    constexpr bool operator>(BasicStringView<CharT> lhs,
                             BasicStringView<CharT> rhs) noexcept {
        return lhs.compare(rhs) > 0;
    }

    template <typename CharT>
    constexpr bool operator>=(BasicStringView<CharT> lhs,
                              BasicStringView<CharT> rhs) noexcept {
        return !(lhs < rhs);
    }

    using StringView = BasicStringView<char>;

    template <typename CharT>
    constexpr typename BasicStringView<CharT>::ConstIterator begin(BasicStringView<CharT> sv) noexcept {
        return sv.begin();
    }

    template <typename CharT>
    constexpr typename BasicStringView<CharT>::ConstIterator end(BasicStringView<CharT> sv) noexcept {
        return sv.end();
    }

    //Hash instance for String type
    template <>
    struct Hash<StringView> {
        constexpr SizeT operator()(StringView data) const noexcept {
            return fnvHash( data.data(), data.size() );
        }
    };
}

constexpr fb::StringView operator "" _sv(const char *str, unsigned long len) noexcept {
    return { str, len };
}
