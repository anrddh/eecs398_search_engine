#pragma once

#include <iostream>

#include "stddef.hpp"
#include "vector.hpp"
#include "string_view.hpp"

namespace fb {
    template <typename CharT>
    class BasicString {
    public:
        using ValueType = CharT;
        using SizeType = SizeT;
        using DifferenceType = PtrDiffT;
        using Reference = Vector<CharT>::Reference;
        using ConstReference = Vector<CharT>::ConstReference;
        using Pointer = Vector<CharT>::Pointer;
        using ConstPointer = Vector<CharT>::ConstPointer;
        using Iterator = Vector<CharT>::Iterator;
        using ConstIterator = Vector<CharT>::ConstIterator;
        using ReverseIterator = Vector<CharT>::ReverseIterator;
        using ConstReverseIterator = Vector<CharT>::ConstReverseIterator;

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

        Reference operator[](SizeType pos) {
            return buf[pos];
        }

        ConstReference operator[](SizeType pos) const {
            return buf[pos];
        }

        Reference front() {
            return buf.front();
        }

        ConstReference front() const {
            return buf.front();
        }

        Reference back() {
            return buf.back();
        }

        ConstReference back() const {
            return buf.back();
        }

        Pointer data() noexcept {
            return buf.data();
        }

        ConstPointer data() const noexcept {
            return buf.data();
        }

        operator BasicStringView<CharT>() const noexcept {
            return BasicStringnView<CharT>(data(), size());
        }

        /* Iterators */

        /* Capacity */
        [[nodiscard]] bool empty() const noexcept {
            return !size();
        }

        [[nodiscard]] SizeType size() const noexcept {
            return buf.size() ? buf.size() - 1 : 0;
        }

        [[nodiscard]] SizeType max_size() const noexcept {
            return buf.max_size() - 1;
        }

        void reserve(SizeType new_cap) {
            buf.reserve(new_cap + 1);
        }

        SizeType capacity() const noexcept {
            return buf.capacity() ? buf.capacity() - 1 : 0;
        }

        void shrink_to_fit() {
            buf.shrink_to_fit();
        }


        //      /* Return a String starting with i and extending for len
        //      characters The substring must be contained within the string.
        //      Values of i and len for valid input are as follows:
        //      i >= 0 && len >= 0 && i <= size && (i + len) <= size.
        //      If both i = size and len = 0, the input is valid and the result is
        //      an empty string.  Throw exception if the input is invalid. */
        //      String substring(int i, int len) const {
        //        if (i < 0 || len < 0 || i > buf_size || (i + len) > buf_size)
        //          throw String_exception("Substring bounds invalid");
        //
        //        String substr;
        //        substr.alloc_mem(len + 1);
        //        for (int j = 0; j < len; ++j)
        //          substr += buf[j + i];
        //        return substr;
        //      }
        //
        //      // Modifiers Set to an empty string with minimum allocation by
        //      // create/swap with an empty string.
        //      void clear() {
        //        String temp("");
        //        swap(temp);
        //      }
        //
        //      /* Remove the len characters starting at i; allocation is
        //      unchanged.  The removed characters must be contained within the
        //      String.  Valid values for i and len are the same as for
        //      substring. */
        //      void remove(int i, int len) {
        //        if (i < 0 || len < 0 || i > size() || (i + len) > size())
        //          throw String_exception("Remove bounds invalid");
        //
        //        for (int j = i + len; j < size(); ++j)
        //          buf[j - len] = buf[j];
        //        buf_size -= len;
        //        buf[buf_size] = 0;
        //      }
        //
        //      /* Insert the supplied source String before character i of this
        //      String, pushing the rest of the contents back, reallocating as
        //      needed.  If i == size, the inserted string is added to the end of
        //      this String.  This function does not create any temporary String
        //      objects.  It either directly inserts the new data into this
        //      String's space if it is big enough, or allocates new space and
        //      copies in the old data with the new data inserted.
        //
        //      This String retains the final allocation.
        //      Throw exception if 0 <= i <= size is false.
        //
        //      The behavior of inserting a String into itself is not specified. */
        //      void insert_before(int i, const String &src) {
        //        if (!src.size())
        //          return;
        //
        //        if (i < 0 || i > size())
        //          throw String_exception("Insertion point out of range");
        //
        //        realloc_mem(src.size());
        //
        //        for (int j = size() - 1 + src.size(); j > i; --j)
        //          buf[j] = buf[j - src.size()];
        //
        //        for (int j = 0; j < src.size(); ++j)
        //          buf[i + j] = src[j];
        //
        //        buf_size += src.size();
        //        buf[buf_size] = 0;
        //      }
        //
        //      /* These concatenation operators add the rhs string data to the
        //      lhs object.  They do not create any temporary String objects. They
        //      either directly copy the rhs data into the lhs space if it is big
        //      enough to hold the rhs, or allocate new space and copy the old lhs
        //      data into it followed by the rhs data. The lhs object retains the
        //      final memory allocation. If the rhs is a null byte or an empty
        //      C-string or String, no change is made to lhs String. */
        //      String &operator+=(char rhs) {
        //        realloc_mem(1);
        //        buf[buf_size++] = rhs;
        //        buf[buf_size] = 0;
        //        return *this;
        //      }
        //
        //      String &operator+=(const char *rhs) {
        //        int rhs_len = strlen(rhs);
        //        realloc_mem(rhs_len);
        //
        //        for (int i = 0; i < rhs_len; ++i)
        //          buf[i + buf_size] = rhs[i];
        //
        //        buf_size += rhs_len;
        //        buf[buf_size] = 0;
        //
        //        return *this;
        //      }
        //
        //      String &operator+=(const String &rhs) { return *this += rhs.c_str(); }
        //
        //      /* Swap the contents of this String with another one.  The member
        //      variable values are interchanged, along with the pointers to the
        //      allocated C-strings, but the two C-strings are neither copied nor
        //      modified. No memory allocation/deallocation is done. */
        //      void swap(String &other) noexcept {
        //        std::swap(buf_size, other.buf_size);
        //        std::swap(alloc, other.alloc);
        //        std::swap(buf, other.buf);
        //      }

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
