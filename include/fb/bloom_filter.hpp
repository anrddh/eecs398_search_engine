#pragma once

#include <fb/stddef.hpp>

template <typename T,
          typename Cont,
          typename Hash,
          fb::SizeT numHashes,
          fb::SizeT size>
class BloomFilter {
public:
    BloomFilter() { }

    void insert(const T &val);
    bool mightContain(const T &val) const;
    bool tryInsert(const T &val);

private:
};
