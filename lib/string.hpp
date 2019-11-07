#pragma once

#include "stddef.hpp"
#include "vector.hpp"
#include "string_view.hpp"
#include "type_traits.hpp"

#include <iostream>
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

        BasicString() = default;

        BasicString(SizeT count, CharT c) : buf(count, c) {
            buf.insert(buf.end(), 0);
        }

        BasicString(const char *cstr_ = "") : buf_size(std::strlen(cstr_)) {
            if (!buf_size)
                return;

            alloc_mem(buf_size + 1);
            std::strcpy(buf, cstr_);
        }

        /*  Element access  */
        Reference at(SizeType pos) {
            if (pos >= buf.size())
                throw std::out_of_range{};
            return buf[pos];
        }

        ConstReference at(SizeType pos) const {
            if (pos >= buf.size())
                throw std::out_of_range{};
            return buf[pos];
        }

        [[nodiscard]] Reference operator[](SizeType pos) {
            return buf[pos];
        }

        [[nodiscard]] ConstReference operator[](SizeType pos) const {
            return buf[pos];
        }

        [[nodiscard]] Reference front() {
            return buf.front();
        }

        [[nodiscard]] ConstReference front() const {
            return buf.front();
        }

        [[nodiscard]] Reference back() {
            return buf.back();
        }

        [[nodiscard]] ConstReference back() const {
            return buf.back();
        }

        [[nodiscard]] Pointer data() noexcept {
            return buf.data();
        }

        [[nodiscard]] ConstPointer data() const noexcept {
            return buf.data();
        }

        operator BasicStringView<CharT>() const noexcept {
            return BasicStringnView<CharT>(data(), size());
        }

        /* Iterators */
        Iterator begin() noexcept {
            return buf.begin();
        }

        ConstIterator begin() const noexcept {
            return buf.begin();
        }

        ConstIterator cbegin() const noexcept {
            return buf.cbegin();
        }

        Iterator end() noexcept {
            return buf.end() - 1;
        }

        ConstIterator end() const noexcept {
            return buf.end() - 1;
        }

        ConstIterator cend() const noexcept {
            return buf.cend() - 1;
        }

        ReverseIterator rbegin() noexcept {
            return buf.rbegin() + 1;
        }

        ConstReverseIterator rbegin() const noexcept {
            return buf.rbegin() + 1;
        }

        ConstReverseIterator crbegin() const noexcept {
            return buf.crbegin() + 1;
        }

        ReverseIterator rend() noexcept {
            return buf.rend();
        }

        ConstReverseIterator rend() const noexcept {
            return buf.rend();
        }

        ConstReverseIterator crend() const noexcept {
            return buf.crend();
        }

        /* Capacity */
        [[nodiscard]] bool empty() const noexcept {
            return !size();
        }

        [[nodiscard]] SizeType size() const noexcept {
            return buf.size() ? buf.size() - 1 : 0;
        }

        void reserve(SizeType new_cap) {
            buf.reserve(new_cap + 1);
        }

        SizeType capacity() const noexcept {
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

        BasicString & append(const BasicStringView<CharT> str) {
            buf.insert(end(), str.begin(), str.end());
            return *this;
        }

        BasicString & append(const BasicString &str,
                             SizeType pos,
                             SizeType count = npos) {
        }

        /*
basic_string& append( const CharT* s, size_type count );
basic_string& append( const CharT* s );
template< class InputIt >
basic_string& append( InputIt first, InputIt last );
basic_string& append( std::initializer_list<CharT> ilist );
	(7) 	(since C++11)
template < class T >
basic_string& append( const T& t );
	(8) 	(since C++17)
template < class T >

basic_string& append( const T& t, size_type pos,
                      size_type count = npos );
	(9) 	(since C++17)
    */
        BasicString substr(SizeType pos = 0, SizeType count = npos) const {
            if (pos > size())
                throw std::out_of_range("");
            return BasicString(data() + pos, count);
        }

        void resize(SizeType count) {
            buf.resize(count + 1);
        }

        void resize(SizeType count, CharT ch) {
            if (count <= size()) {
                buf.resize(count + 1);
                buf.last() = 0;
                return;
            }


        }

        void swap(BasicString &other) noexcept {
            swap(buf, other.buf);
        }

    private:
        Vector<CharT> buf;
    };

    using String = BasicString<char>;

// non-member overloaded operators

// compare lhs and rhs strings; constructor will convert a C-string literal to a String.
// comparison is based on std::strcmp result compared to 0
bool operator== (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
}

bool operator!= (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) != 0;
}

bool operator< (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool operator> (const String& lhs, const String& rhs) {
    return std::strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

/* Concatenate a String with another String.  If one of the arguments
 is a C-string, the String constructor will automatically create a
 temporary String for it to match this function (inefficient, but
 instructive).  This automatic behavior would be disabled if the
 String constructor was declared "explicit".  This function constructs
 a copy of the lhs in a local String variable, then concatenates the
 rhs to it with operator +=, and returns it. */
String operator+ (const String &lhs, const String &rhs) {
    String temp(lhs);
    temp += rhs;
    return temp;
}

// Input and output operators and functions The output operator writes
// the contents of the String to the stream
std::ostream & operator<< (std::ostream &os, const String &str) {
    os << str.c_str();
    return os;
}

/* The input operator clears the supplied String, then starts reading
the stream.  It skips initial whitespace, then copies characters into
the supplied str until whitespace is encountered again. The
terminating whitespace remains in the input stream, analogous to how
string input normally works.  str is expanded as needed, and retains
the final allocation.  If the input stream fails, str contains
whatever characters were read. */
std::istream & operator>> (std::istream &is, String &str) {
    str.clear();

    // skip initial whitespace
    char c;
    while (is && (c = is.peek()) && isspace(c))
        is.get();

    while (is && (c = is.peek()) && !isspace(c))
        str += is.get();

    return is;
}

/* getline for String clears str to an empty String, then reads
characters into str until it finds a '\n'.  Similar to std::getline,
the newline character is consumed, but not stored in the String.
str's allocation is expanded as needed, and it retains the final
allocation.  If the input stream fails, str contains whatever
characters were read. */
std::istream & getline(std::istream &is, String &str) {
    str.clear();

    char c;
    while (is && (c = is.peek()) && c != '\n') {
        c = is.get();
        str += c;
    }

    if (c == '\n')
        is.get(); // read in last newline

    return is;
}
}
