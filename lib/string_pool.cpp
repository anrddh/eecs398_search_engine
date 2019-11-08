// Created by Jaeyoon Kim 11/7/19
//#pragma once
#include "stddef.hpp"
#include "SavedStrings.hpp"
#include "url_pool.hpp"
#include "string_pool.hpp"
//#include "string.hpp"
#include <string>
#define String std::string

namespace fb {

template <SizeT N>
SizeT StringPool<N>::get_offset( String str ) {
   SizeT hash = hasher(str);
   AutoLock l(offset_hashes[hash % N].second);
   offset_hashes[hash % N].first.find( str, hash );
}

template <SizeT N>
char* StringPool<N>::access_offset( SizeT offset ) {
   return str_array.get_str(offset);
}

}; //fb
