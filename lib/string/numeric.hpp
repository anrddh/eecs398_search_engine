#pragma once

#include "main.hpp"

#include <stdlib.h>

namespace fb {

    int stoi(const String &str, SizeT *pos = 0, int base = 10) {
        char *pos_ = reinterpret_cast<char *>(pos);
        return static_cast<int>(strtol(str.data(),
                                       &pos_,
                                       base));
    }

    long stol(const String &str, SizeT *pos = 0, int base = 10) {
        char *pos_ = reinterpret_cast<char *>(pos);
        return strtol(str.data(), &pos_, base);
    }

    long long stoll(const String &str, SizeT *pos = 0, int base = 10) {
            char *pos_ = reinterpret_cast<char *>(pos);
            return strtoll(str.data(), &pos_, base);
    }

};
