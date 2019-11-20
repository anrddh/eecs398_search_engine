#pragma once

#include <fb/string_view.hpp>
#include <fb/exception.hpp>
#include <fb/utility.hpp>

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
        auto rval = regcomp(&t, regex, 0);
        if (rval) {
            fb::String str;
            str.resize(regerror(rval, &t, nullptr, 0) - 1);
            auto numWritten = regerror(rval, &t, str.data(), str.size() + 1);
            assert(numWritten == str.size() - 1);
            throw Error(str);
        }
        initd = true;
    }

    bool match(const char *needle) {
        assert(initd);
        auto rval = regexec(&t, needle, 0, nullptr, REG_EXTENDED);
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
        fb::swap(initd, other.initd);
    }

    ~Regex() noexcept {
        if (initd)
            regfree(&t);
    }

private:
    regex_t t;
    bool initd = false;
};

template <>
inline void fb::swap(Regex &lhs, Regex &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
