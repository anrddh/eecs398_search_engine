#pragma once

#include "../iterator.hpp"
#include "misc.hpp"

#include <utility>

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

    template <typename It, typename T>
    void uninitializedFill(It first, It last, const T &value)
    {
        auto current = first;
        try {
            while (current != last)
                ::new (static_cast<void *>(addressof(*current++)))
                      typename IteratorTraits<It>::ValueType(value);
        }  catch (...) {
            destroy(first, current);
            throw;
        }
    }

    template <typename It>
    void uninitializedDefaultConstruct(It first, It last) {
        auto current = first;
        try {
            while (current != last)
                ::new (static_cast<void *>(addressof(*current++)))
                      typename IteratorTraits<It>::ValueType;
        }  catch (...) {
            destroy(first, current);
            throw;
        }
    }

    template<class It, class DestIt>
    DestIt uninitializedMove(It first, It last, DestIt dest) {
        auto current = dest;
        try {
            while (first != last)
                ::new (static_cast<void *>(addressof(*current++)))
                      typename IteratorTraits<It>
                      ::ValueType(std::move(*first++));
            return current;
        } catch (...) {
            destroy(dest, current);
            throw;
        }
    }
};
