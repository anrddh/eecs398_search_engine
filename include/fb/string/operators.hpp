#pragma once

#include "main.hpp"
#include "../stddef.hpp"
#include "../functional.hpp"

#include <iostream>

#include <stdio.h>
#include <assert.h>

namespace fb {
    inline bool operator== (const String &lhs, const StringView rhs) {
        return lhs.compare( rhs ) == 0;
    }

    inline bool operator!= (const String &lhs, const StringView rhs) {
        return !( lhs == rhs );
    }

    inline std::ostream & operator<< (std::ostream &os, const String &str) {
        os << str.data();
        return os;
    }

    inline String operator+ (const String &lhs, const StringView rhs) {
        String temp(lhs);
        temp += rhs;
        return temp;
    }

    inline String operator+ (const String &lhs, const String &rhs) {
        String temp(lhs);
        temp += rhs;
        return temp;
    }

    inline String operator+ (const String &lhs, const char rhs) {
        String temp(lhs);
        temp.pushBack(rhs);
        return temp;
    }

    inline String operator+ (const String &lhs, const char *rhs) {
        String temp(lhs);
        temp += rhs;
        return temp;
    }

    inline std::istream & operator>> (std::istream &is, String &str) {
        str.clear();

        // skip initial whitespace
        char c;
        while (is && (c = is.peek()) && isspace(c))
            is.get();

        std::cout << "Skipped whitespace\n";

        while (is && (c = is.peek()) && !isspace(c))
            str += is.get();

        std::cout << str;

        return is;
    }

    inline std::istream & getline(std::istream &is, String &str) {
      str.clear();

      char c = 0;
      while (is && (c = is.peek()) && c != '\n') {
          c = is.get();
          str += c;
      }

      if (c == '\n')
          is.get(); // read in last newline

      return is;
    }

    //Hash instance for String type
    template <>
    struct Hash<String> {
        SizeT operator() (const String &data) const noexcept {
            return fnvHash( data.data(), data.size() );
        }
    };

    template <>
    struct HashPairGen<String> {
        Pair<SizeT, SizeT> operator() (const String &data) const noexcept {
            return fnvHashPair( data.data(), data.size() );
        }
    };

    struct ToStringConvErr : public Exception {
        ToStringConvErr(const char *msg_) : Exception(msg_) {}
    };


    inline String toString(int value) {
        auto num = snprintf(nullptr, 0, "%d", value);
        if (num <= 0)
            throw ToStringConvErr("");

        String str;
        str.resize(num);
        auto numWritten = snprintf(str.data(), str.size() + 1, "%d", value);
        assert(numWritten == static_cast<long long>(str.size()));
        return str;
    }

    inline String toString(unsigned long value) {
        auto num = snprintf(nullptr, 0, "%lu", value);
        if (num <= 0)
            throw ToStringConvErr("");

        String str;
        str.resize(num);
        auto numWritten = snprintf(str.data(), str.size() + 1, "%lu", value);
        assert(numWritten == static_cast<long long>(str.size()));
        return str;
    }

    inline String toString(unsigned long long value) {
        auto num = snprintf(nullptr, 0, "%llu", value);
        if (num <= 0)
            throw ToStringConvErr("");

        String str;
        str.resize(num);
        auto numWritten = snprintf(str.data(), str.size() + 1, "%llu", value);
        assert(numWritten == static_cast<long long>(str.size()));
        return str;
    }

    inline String toString(double value) {
        auto num = snprintf(nullptr, 0, "%f", value);
        if (num <= 0)
            throw ToStringConvErr("");

        String str;
        str.resize(num);
        auto numWritten = snprintf(str.data(), str.size() + 1, "%f", value);
        assert(numWritten == static_cast<long long>(str.size()));
        return str;
    }
}
