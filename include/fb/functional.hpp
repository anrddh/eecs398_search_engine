//Created by Chandler Meyers 11/1/2019
//Edited by Chandler Meyers 11/5/2019
#pragma once

#include <fb/stddef.hpp>
#include <fb/utility.hpp>

namespace fb {

constexpr SizeT fnvHashState(const char *data,
                             SizeT length,
                             SizeT hash) noexcept {
    constexpr SizeT FnvPrime = 1099511628211ull;
    for( SizeT i = 0; i < length; ++i )
    {
        hash *= FnvPrime;
        hash ^= data[ i ];
    }
    return hash;
}

//This hash function for char buffers from lecture slides
constexpr SizeT fnvHash( const char *data, SizeT length ) noexcept {
    return fnvHashState(data, length, 146959810393466560ull);
}

inline Pair<SizeT, SizeT> fnvHashPair(const char *data, SizeT length) {
    auto hash1 = fnvHash(data, length);
    auto hash2 = fnvHashState(data, length, hash1);
    return { hash1, hash2 };
}

template <typename T>
struct HashPairGen {};

template <typename T>
struct Hash {};

//Hash instance for SizeT type
template <>
struct Hash<SizeT> {
    // Jin Soo removed constexpr
    SizeT operator() ( const SizeT &data ) const noexcept {
        return fnvHash( (char *) &data, sizeof(data) );
    }
};

//Equality predicate for template type which supports ==
template <typename T = void> struct EqualTo {
    constexpr bool operator() (const T &lhs, const T &rhs) const noexcept {
        return lhs == rhs;
    }
    using FirstArgumentType = T;
    using SecondArgumentType = T;
    using ResultType = bool;
};

//Inequality predicate for template type which supports !=
template <typename T> struct NotEqualTo {
    constexpr bool operator() (const T &lhs, const T &rhs) const noexcept {
        return lhs != rhs;
    }
    using FirstArgumentType = T;
    using SecondArgumentType = T;
    using ResultType = bool;
};

//Less than predicate for template type which supports <
template <typename T> struct Less {
    constexpr bool operator() (const T &lhs, const T &rhs) const noexcept {
        return lhs < rhs;
    }
    using FirstArgumentType = T;
    using SecondArgumentType = T;
    using ResultType = bool;
};

//Greater than predicate for template type which supports >
template <typename T> struct Greater {
    constexpr bool operator() (const T &lhs, const T &rhs) const noexcept {
        return lhs > rhs;
    }
    using FirstArgumentType = T;
    using SecondArgumentType = T;
    using ResultType = bool;
};

//Less than or equal to predicate for template type which supports <=
template <typename T> struct LessEqual {
    constexpr bool operator() (const T &lhs, const T &rhs) const noexcept {
        return lhs <= rhs;
    }
    using FirstArgumentType = T;
    using SecondArgumentType = T;
    using ResultType = bool;
};

//Greater than or equal to predicate for template type which supports <
template <typename T> struct GreaterEqual {
    constexpr bool operator() (const T &lhs, const T &rhs) const noexcept {
        return lhs >= rhs;
    }
    using FirstArgumentType = T;
    using SecondArgumentType = T;
    using ResultType = bool;
};

} //fb
