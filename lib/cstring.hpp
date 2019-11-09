#pragma once

#include "stddef.hpp"

namespace fb {

    constexpr SizeT strlen(const char *str) noexcept {
        auto begin = str;
        while (*str++)
            ;
        return str - begin - 1;
   }

    constexpr SizeT strnlen(const char *str, SizeT maxlen) noexcept {
        auto begin = str;
        while (maxlen-- && *str++ )
            ;
        return str - begin;
   }

    constexpr char * strcpy(const char *src, char *dest) noexcept {
        while ((*dest++ = *src++))
            ;
        return dest;
    }

    constexpr int strcmp(const char *lhs, const char *rhs) noexcept {
        auto lhsp = (const unsigned char *) lhs;
        auto rhsp = (const unsigned char *) rhs;

        for ( ; *lhsp && (*lhsp == *rhsp); ++lhsp, ++rhsp)
            ;
        return *lhsp - *rhsp;
    }

    constexpr int strncmp(const char *lhs, const char *rhs, SizeT n) noexcept {
        auto lhsp = (const unsigned char *) lhs;
        auto rhsp = (const unsigned char *) rhs;

        for ( ; n; ++lhsp, ++rhsp, --n)
            if (*lhsp != *rhsp)
                return *lhsp - *rhsp;
        return 0;
    }

    constexpr char * strnstr(const char *haystack,
                             const char *needle,
                             SizeT len) noexcept {
        SizeT needlen = strnlen(needle, len);
        if (!needlen)
            return const_cast<char *>(haystack);

        for ( ; needlen <= len; --len, ++haystack)
            if (!strncmp(haystack, needle, needlen))
                return const_cast<char *>(haystack);

        return nullptr;
    }

    constexpr int memcmp(const void *s1, const void *s2, SizeT n) noexcept {
        const unsigned char *s1p = static_cast<const unsigned char *>(s1);
        const unsigned char *s2p = static_cast<const unsigned char *>(s2);

        while (n--)
            if (*s1p++ != *s2p++)
                return *(s1p - 1) - *(s2p - 1);

        return 0;
    }

}
