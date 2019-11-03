//Created by Chandler Meyers 11/1/2019
#pragma once

#include "string.hpp"
#include "stddef.hpp"
#define INITIAL_SIZE 1024

//Implementation of features typically from std::functional
//Add things to this as needed

namespace fb {

//This hash function for char buffers from lecture slides
SizeT fnvHash( const char *data, SizeT length )
{
    static const SizeT FnvOffsetBasis = 146959810393466560;
    static const SizeT FnvPrime = 1099511628211ul;
    SizeT hash = FnvOffsetBasis;
    for( SizeT i = 0; i < length; ++i )
    {
        hash *= FnvPrime;
        hash ^= data[ i ];
    }
    return hash;
}

template <class T>
struct Hash;

//Hash instance for String type
template <>
struct Hash<String> {
    SizeT operator() ( const String &data ) const {
        return fnvHash( data.c_str(), data.size() );
    }
};

//Hash instance for SizeT type
template <>
struct Hash<SizeT> {
    SizeT operator() ( const SizeT &data ) const {
        return fnvHash( (char *) &data, sizeof(data) );
    }
};

//Equality predicate for template type which supports ==
template <class T = void> struct EqualTo {
    constexpr bool operator() (const T &lhs, const T &rhs) const {
        return lhs == rhs;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

//Inequality predicate for template type which supports !=
template <class T> struct NotEqualTo {
    constexpr bool operator() (const T &lhs, const T &rhs) const {
        return lhs != rhs;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

//Less than predicate for template type which supports <
template <class T> struct Less {
    constexpr bool operator() (const T &lhs, const T &rhs) const {
        return lhs < rhs;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

//Greater than predicate for template type which supports >
template <class T> struct Greater {
    constexpr bool operator() (const T &lhs, const T &rhs) const {
        return lhs > rhs;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

//Less than or equal to predicate for template type which supports <=
template <class T> struct LessEqual {
    constexpr bool operator() (const T &lhs, const T &rhs) const {
        return lhs <= rhs;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

//Greater than or equal to predicate for template type which supports <
template <class T> struct GreaterEqual {
    constexpr bool operator() (const T &lhs, const T &rhs) const {
        return lhs >= rhs;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef bool result_type;
};

} //fb
