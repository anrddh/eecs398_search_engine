#pragma once

#include "main.hpp"

#include <stdlib.h>

namespace fb {

    inline int stoi(const String &str, SizeT *pos = 0, int base = 10) {
        char *pos_ = reinterpret_cast<char *>(pos);
        return static_cast<int>(strtol(str.data(),
                                       &pos_,
                                       base));
    }

    inline long stol(const String &str, SizeT *pos = 0, int base = 10) {
        char *pos_ = reinterpret_cast<char *>(pos);
        return strtol(str.data(), &pos_, base);
    }

    inline long long stoll(const String &str, SizeT *pos = 0, int base = 10) {
            char *pos_ = reinterpret_cast<char *>(pos);
            return strtoll(str.data(), &pos_, base);
    }

}
