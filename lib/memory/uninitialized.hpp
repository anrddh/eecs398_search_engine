#pragma once

#include "../iterator.hpp"
#include "misc.hpp"

namespace fb {

    template <typename T>
    void destroyAt(T *p) {
        p->~T();
    }

    template <typename It>
    void destroy(It first, It last) {
        for (; first != last; ++first)
            destroyAt(addressof(*first));
    }

    template <typename It, typename DIt>
    It uninitializedCopy(It first, It last, DIt dest) {
        auto current = dest;
        try {
            while (first != last)
                ::new (addressof(*current++))
                      typename IteratorTraits<DIt>::ValueType(*first++);
            return current;
        } catch (...) {
            destroy(dest, current);
            throw;
        }
    }

};
