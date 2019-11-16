#pragma once

#include "main.hpp"
#include "../stddef.hpp"
#include "../functional.hpp"

#include <iostream>

namespace fb {
    inline String operator+ (const String &lhs, const StringView rhs) {
        String temp(lhs);
        temp += rhs;
        return temp;
    }

    inline std::ostream & operator<< (std::ostream &os, const String &str) {
        os << str.data();
        return os;
    }

    inline std::istream & operator>> (std::istream &is, String &str) {
        str.clear();

        // skip initial whitespace
        char c;
        while (is && (c = is.peek()) && isspace(c))
            is.get();

        while (is && (c = is.peek()) && !isspace(c))
            str += is.get();

        return is;
    }

    inline std::istream & getline(std::istream &is, String &str) {
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

    //Hash instance for String type
    template <>
    struct Hash<BasicString<char>> {
        SizeT operator() (const BasicString<char> &data) const noexcept {
            return fnvHash( data.data(), data.size() );
        }
    };
}
