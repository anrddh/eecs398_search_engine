#pragma once

#include "../iterator.hpp"
#include "misc.hpp"

#include <utility>
#include <new>

namespace fb {

    template <typename T>
    void destroyAt(T *p) {
        p->~T();
    }

    template <typename It>
    void destroy(It first, It last) {
        for (; first != last; ++first)
            fb::destroyAt(fb::addressof(*first));
    }

    template <typename It, typename DIt>
    It uninitializedCopy(It first, It last, DIt dest) {
        auto current = dest;
        try {
            while (first != last)
                ::new (current++) typename IteratorTraits<DIt>::ValueType(*first++);
            return current;
        } catch (...) {
            fb::destroy(dest, current);
            throw;
        }
    }

    template <typename It, typename T>
    void uninitializedFill(It first, It last, const T &value)
    {
        auto current = first;
        try {
            while (current != last)
                ::new (current++) typename IteratorTraits<It>::ValueType(value);
        }  catch (...) {
            fb::destroy(first, current);
            throw;
        }
    }

    template <typename It>
    void uninitializedDefaultConstruct(It first, It last) {
        auto current = first;
        try {
            while (current != last)
                ::new (fb::addressof(*current++)) typename IteratorTraits<It>::ValueType;
        }  catch (...) {
            fb::destroy(first, current);
            throw;
        }
    }

    template<class It, class DestIt>
    DestIt uninitializedMove(It first, It last, DestIt dest) {
        DestIt current = dest;
        try {
            while (first != last)
                new (fb::addressof(*current++))
                    typename IteratorTraits<DestIt>::ValueType(std::move(*first++));
            return current;
        } catch (...) {
            fb::destroy(dest, current);
            throw;
        }
    }
}
