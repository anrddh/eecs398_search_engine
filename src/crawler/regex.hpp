#pragma once

#include "../../lib/string_view.hpp"
#include "../../lib/Exception.hpp"
#include "../../lib/utility.hpp"

#include <assert.h>
#include <regex.h>

class Regex {
public:
    struct Error : fb::Exception {
        Error(const fb::String &msg) : fb::Exception(msg) {}
    };

    Regex(const char *regex) {
        auto rval = regcomp(&t, regex, REG_ICASE);
        if (rval) {
            String str;
            str.resize(regerror(rval, &regex, nullptr, 0) - 1);
            auto numWritten = regerror(rval, &regex, str.data(), str.size() + 1);
            assert(numWritten == str.size() - 1);
            throw Error(str);
        }
    }

    bool match(const char *needle) {
        auto rval = regexec(t, needle, 0, nullptr, 0);
        return !rval;
    }

    constexpr Regex(Regex &&other) noexcept {
        swap(other);
    }

    constexpr Regex & operator=(Regex &&other) noexcept {
        swap(other);
        return *this;
    }

    constexpr void swap(Regex &other) noexcept {
        fb::swap(t, other.t);
    }

    ~Regex() noexcept {
        if (t)
            regfree(t);
    }

private:
    regex_t *t = nullptr;
};

template <>
inline void fb::swap(Regex &lhs, Regex &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
