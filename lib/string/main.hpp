#pragma once

#include "../stddef.hpp"
#include "../vector.hpp"
#include "../string_view.hpp"
#include "../type_traits.hpp"
#include "../cstring.hpp"
#include "../functional.hpp"

#include <type_traits>
#include <initializer_list>
#include <stdexcept>

namespace fb {
    template <typename CharT>
    class BasicString {
    public:
        using ValueType = CharT;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;
        using Reference = typename Vector<CharT>::Reference;
        using ConstReference = typename Vector<CharT>::ConstReference;
        using Pointer = typename Vector<CharT>::Pointer;
        using ConstPointer = typename Vector<CharT>::ConstPointer;
        using Iterator = typename Vector<CharT>::Iterator;
        using ConstIterator = typename Vector<CharT>::ConstIterator;
        using ReverseIterator = typename Vector<CharT>::ReverseIterator;
        using ConstReverseIterator = typename Vector<CharT>::ConstReverseIterator;

        static constexpr SizeType npos = SizeType(-1);

        BasicString() = default;

        BasicString(SizeT count, CharT c) : buf(count, c) {
            buf.pushBack(0);
        }

        BasicString(const char *cstr, SizeType size) : buf(cstr, cstr + size) {
            buf.pushBack(0);
        }

        BasicString(const char *cstr) : BasicString(cstr, fb::strlen(cstr)) {}

        /*  Element access  */
        Reference at(SizeType pos) {
            if (pos >= buf.size())
                throw std::out_of_range("");
            return buf[pos];
        }

        ConstReference at(SizeType pos) const {
            if (pos >= buf.size())
                throw std::out_of_range("");
            return buf[pos];
        }

        [[nodiscard]] constexpr Reference operator[](SizeType pos) {
            return buf[pos];
        }

        [[nodiscard]] constexpr ConstReference operator[](SizeType pos) const {
            return buf[pos];
        }

        [[nodiscard]] Reference front() {
            return buf.front();
        }

        [[nodiscard]] ConstReference front() const {
            return buf.front();
        }

        [[nodiscard]] constexpr Reference back() {
            return buf.back();
        }

        [[nodiscard]] constexpr ConstReference back() const {
            return buf.back();
        }

        [[nodiscard]] constexpr Pointer data() noexcept {
            return buf.data();
        }

        [[nodiscard]] constexpr ConstPointer data() const noexcept {
            return buf.data();
        }

        operator BasicStringView<CharT>() const noexcept {
            return BasicStringView<CharT>(data(), size());
        }

        /* Iterators */
        constexpr Iterator begin() noexcept {
            return buf.begin();
        }

        constexpr ConstIterator begin() const noexcept {
            return buf.begin();
        }

        [[nodiscard]] constexpr ConstIterator cbegin() const noexcept {
            return buf.cbegin();
        }

        [[nodiscard]] constexpr Iterator end() noexcept {
            return buf.end() - 1;
        }

        [[nodiscard]] constexpr ConstIterator end() const noexcept {
            return buf.end() - 1;
        }

        [[nodiscard]] constexpr ConstIterator cend() const noexcept {
            return buf.cend() - 1;
        }

        [[nodiscard]] constexpr ReverseIterator rbegin() noexcept {
            return buf.rbegin() + 1;
        }

        [[nodiscard]] constexpr ConstReverseIterator rbegin() const noexcept {
            return buf.rbegin() + 1;
        }

        [[nodiscard]] constexpr ConstReverseIterator crbegin() const noexcept {
            return buf.crbegin() + 1;
        }

        [[nodiscard]] constexpr ReverseIterator rend() noexcept {
            return buf.rend();
        }

        [[nodiscard]] constexpr ConstReverseIterator rend() const noexcept {
            return buf.rend();
        }

        [[nodiscard]] constexpr ConstReverseIterator crend() const noexcept {
            return buf.crend();
        }

        /* Capacity */
        [[nodiscard]] constexpr bool empty() const noexcept {
            return !size();
        }

        [[nodiscard]] constexpr SizeType size() const noexcept {
            return buf.size() ? buf.size() - 1 : 0;
        }

        void reserve(SizeType new_cap) {
            buf.reserve(new_cap + 1);
        }

        [[nodiscard]] constexpr SizeType capacity() const noexcept {
            return buf.capacity() ? buf.capacity() - 1 : 0;
        }

        /* Operations */
        void clear() noexcept {
            buf.clear();
        }

        BasicString & insert(SizeType index, SizeType count, CharT ch) {
            if (index > size())
                throw std::out_of_range("");
            buf.insert(buf.begin() + index, count, ch);
            return *this;
        }

        BasicString & insert(SizeType index, const BasicStringView<CharT> s) {
            if (index > size())
                throw std::out_of_range("");
            return insert(index, s, s.size());
        }

        BasicString & insert(SizeType index,
                             const BasicStringView<CharT> s,
                             SizeType count) {
            if (index > size())
                throw std::out_of_range("");
            buf.insert(buf.begin() + index, s.begin(), s.begin() + count);
            return *this;
        }

        BasicString & insert(SizeType index, const BasicString &s) {
            if (index > size())
                throw std::out_of_range("");
            buf.insert(buf.begin() + index, s.begin(), s.end());
            return *this;
        }

        BasicString & insert(SizeType index, const BasicString &str,
                             SizeType index_str, SizeType count = npos) {
            if (index > size() || index_str > str.size())
                throw std::out_of_range("");
            return insert(index, str.substr(index_str, count));
        }

        Iterator insert(ConstIterator pos, CharT ch) {
            return buf.insert(pos, ch);
        }

        Iterator insert(ConstIterator pos, SizeType count, CharT ch) {
            return buf.insert(pos, count, ch);
        }

        template <typename It>
        Iterator insert(ConstIterator pos,
                        EnableIf<std::is_base_of_v<
                        InputIteratorTag,
                        typename IteratorTraits<It>::IteratorCategory
                        >, It>
                        first, It last) {
            return buf.insert(pos, first, last);
        }

        Iterator insert(ConstIterator pos, std::initializer_list<CharT> ilist) {
            return insert(pos, ilist.begin(), ilist.end());
        }

        Iterator erase(ConstIterator position) {
            return buf.erase(position);
        }

        Iterator erase(ConstIterator first, ConstIterator last) {
            return buf.erase(first, last);
        }

        void pushBack(CharT ch) {
            insert(end() - 1, ch);
        }

        void popBack() {
            erase(end() - 1);
        }

        BasicString & append(SizeType count, CharT ch) {
            buf.insert(end(), count, ch);
            return *this;
        }

        BasicString & append(const BasicStringView<CharT> str, SizeType count) {
            buf.insert(end(), str.begin(), str.begin() + count);
            return *this;
        }

        BasicString & append(const BasicStringView<CharT> str) {
            buf.insert(end(), str.begin(), str.end());
            return *this;
        }

        BasicString & append(const BasicStringView<CharT> str,
                             SizeType pos,
                             SizeType count = npos) {
            if (pos > str.size())
                throw std::out_of_range("");

            auto first = str.begin() + pos;

            if (count == npos || pos + count >= str.size())
                buf.insert(end(), first, str.end());
            else
                buf.insert(end(), first, pos + count);

            return *this;
        }

        template <typename It>
        BasicString & append(EnableIfT<
                             std::is_base_of_v<InputIteratorTag,
                             typename IteratorTraits<It>::IteratorCategory>,
                             It> first, It last) {
            buf.insert(end(), first, last);
            return *this;
        }

        BasicString & append(std::initializer_list<CharT> ilist) {
            return append(ilist.begin(), ilist.end());
        }

        BasicString & operator+=(const BasicStringView<CharT> s) {
            return append(s);
        }

        BasicString & operator+=(CharT ch) {
            insert(end(), ch);
            return *this;
        }

        BasicString & operator+=(std::initializer_list<CharT> ilist) {
            return append(ilist.begin(), ilist.end());
        }

        constexpr int compare(const BasicStringView<CharT> str) const noexcept {
            return BasicStringView<CharT>(*this).compare(str);
        }

        bool startsWith(const BasicStringView<CharT> str) const noexcept {
            return BasicStringView<CharT>(*this).startsWith(str);
        }

        bool startsWith(CharT x) const noexcept {
            return BasicStringView<CharT>(*this).startsWith(x);
        }

        bool endsWith(const BasicStringView<CharT> str) const noexcept {
            return BasicStringView<CharT>(*this).endsWith(str);
        }

        bool endsWith(CharT x) const noexcept {
            return BasicStringView<CharT>(*this).endsWith(x);
        }

        BasicString substr(SizeType pos = 0, SizeType count = npos) const {
            if (pos > size())
                throw std::out_of_range("");
            return BasicString(data() + pos, count);
        }

        SizeType copy(CharT *dest,
                      SizeType count,
                      SizeType pos = 0) const {
            return BasicStringView<CharT>(*this).copy(dest, count, pos);
        }


        void resize(SizeType count) {
            buf.resize(count + 1);
        }

        void resize(SizeType count, CharT ch) {
            if (count <= size()) {
                buf.resize(count + 1);
                return;
            }

            buf.last() = ch;
            buf.reserve(count + 1);
            buf.resize(count, ch);
            buf.push_back(0);
        }

        constexpr void swap(BasicString &other) noexcept {
            swap(buf, other.buf);
        }

        constexpr SizeType find(const BasicString &str, SizeType pos = 0 ) const noexcept {
            return StringView(*this).find(str, pos);
        }

        constexpr SizeType find(const CharT *s, SizeType pos, SizeType count) const {
            return StringView(*this).find(s,pos,count);
        }

        constexpr SizeType find(const CharT *s, SizeType pos = 0) const {
            return StringView(*this).find(s,pos);
        }

        constexpr SizeType find(CharT ch, SizeType pos = 0) const noexcept {
            return StringView(*this).find(ch, pos);
        }

    private:
        Vector<CharT> buf;
    };

    using String = BasicString<char>;


}
