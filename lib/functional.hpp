//Created by Chandler Meyers 11/1/2019
//Edited by Chandler Meyers 11/5/2019
#pragma once

#include "string.hpp"
#include "stddef.hpp"

<<<<<<< HEAD
#define INITIAL_SIZE 1024

=======
>>>>>>> Fix compile errors
//Implementation of features typically from std::functional
//Add things to this as needed

namespace fb {

//This hash function for char buffers from lecture slides
<<<<<<< HEAD
SizeT fnvHash( const char *data, SizeT length )
=======
constexpr SizeT fnvHash( const char *data, SizeT length ) noexcept
>>>>>>> 493d51dfaba6c0df9dbb667cf830de38e3e31557
{
    constexpr SizeT FnvOffsetBasis = 146959810393466560;
    constexpr SizeT FnvPrime = 1099511628211ul;
    SizeT hash = FnvOffsetBasis;
    for( SizeT i = 0; i < length; ++i )
    {
        hash *= FnvPrime;
        hash ^= data[ i ];
    }
    return hash;
}

template <typename T>
struct Hash;

<<<<<<< HEAD
//Hash instance for String type
template <>
struct Hash<String> {
<<<<<<< HEAD
    SizeT operator() ( const String &data ) const {
=======
    constexpr SizeT operator() ( const String &data ) const noexcept {
>>>>>>> 493d51dfaba6c0df9dbb667cf830de38e3e31557
        return fnvHash( data.c_str(), data.size() );
    }
};

=======
>>>>>>> Fix compile errors
//Hash instance for SizeT type
template <>
struct Hash<SizeT> {
<<<<<<< HEAD
    SizeT operator() ( const SizeT &data ) const {
=======
    constexpr SizeT operator() ( const SizeT &data ) const noexcept {
>>>>>>> 493d51dfaba6c0df9dbb667cf830de38e3e31557
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
