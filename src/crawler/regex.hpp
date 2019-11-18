#pragma once

#include "../../lib/string_view.hpp"
#include "../../lib/Exception.hpp"
#include "../../lib/utility.hpp"

#include <assert.h>
#include <regex.h>
#include <string.h>

class Regex {
public:
    struct Error : fb::Exception {
        Error(const fb::String &msg) : fb::Exception(msg) {}
    };

    Regex() {
        memset(&t, 0, sizeof(t));
    }

    Regex(const char *regex) {
        auto rval = regcomp(&t, regex, REG_ICASE);
        if (rval) {
            fb::String str;
            str.resize(regerror(rval, &t, nullptr, 0) - 1);
            auto numWritten = regerror(rval, &t, str.data(), str.size() + 1);
            assert(numWritten == str.size() - 1);
            throw Error(str);
        }
    }

    bool match(const char *needle) {
        auto rval = regexec(&t, needle, 0, nullptr, 0);
        return !rval;
    }

    Regex(Regex &&other) noexcept {
        swap(other);
    }

    Regex & operator=(Regex &&other) noexcept {
        swap(other);
        return *this;
    }

    void swap(Regex &other) noexcept {
        fb::swap(t, other.t);
    }

    ~Regex() noexcept {
        regfree(&t);
    }

private:
    regex_t t;
};

template <>
inline void fb::swap(Regex &lhs, Regex &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
