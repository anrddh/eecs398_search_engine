#pragma once

#include <disk/disk_vec.hpp>

#include <fb/stddef.hpp>
#include <fb/mutex.hpp>
#include <fb/string.hpp>
#include <fb/functional.hpp>
#include <fb/type_traits.hpp>

#include <utility>

#include <stdint.h>

/* Thread safe implementation of bloom filter */
template <uint8_t numHashes,
          fb::SizeT size,
          template <typename> typename Cont = DiskVec,
          typename T = fb::String,
          typename HashPairGen = fb::HashPairGen<T>>
class BloomFilter {
public:
    template <typename ... Args>
    BloomFilter(Args && ... args)
        : cont(std::forward<Args>(args)...) {

        static_assert(!(size % 8));
        if constexpr (!fb::IsSameV<Cont<T>, DiskVec<T>>) {
            cont.resize(size / 8);
        }
    }

    void insert(const T &val) {
        computeHashes(val);
        fb::AutoLock l(m);
        for (fb::SizeT i = 0; i < numHashes; ++i)
            set( hashes[ i ]);
    }

    bool mightContain(const T &val) {
        computeHashes(val);
        fb::AutoLock l(m);
        for (fb::SizeT i = 0; i < numHashes; ++i)
            if (!get( hashes[ i ]))
                return false;
        return true;
    }

    bool tryInsert(const T &val) {
        computeHashes(val);
        fb::AutoLock l(m);
        for (fb::SizeT i = 0; i < numHashes; ++i) {
            if (!get( hashes[ i ])) {
                for (fb::SizeT j = i; j < numHashes; ++j)
                    set( hashes[ j ]);
                return true;
            }
        }
        return false;
    }

private:
    constexpr uint64_t ithhash(uint8_t i,
                               uint64_t hash1,
                               uint64_t hash2) noexcept {
        return (hash1 + i * hash2) % size;
    }

    constexpr void computeHashes(const T &val) noexcept {
        auto [hash1, hash2] = gen(val);
        for (uint8_t i = 0; i < numHashes; ++i)
            hashes[i] = ithhash(i, hash1, hash2);
    }

    void set(fb::SizeT idx) {
        cont[idx/8] |= (1 << (idx % 8));
    }

    bool get(fb::SizeT idx) {
        return cont[idx/8] & (1 << (idx % 8));
    }

    Cont<uint8_t> cont;
    fb::Mutex m;
    HashPairGen gen;
    uint64_t hashes[numHashes];
};
