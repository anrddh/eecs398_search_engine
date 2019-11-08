// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "stddef.hpp"
#include "SavedStrings.hpp"
#include "unordered_set.hpp"
//#include "url_pool.hpp"
#include "offset_lookup.hpp"
#include "mutex.hpp"
#include "utilities.hpp" // for pair
//#include "string.hpp"
#include <string>
#define String std::string

namespace fb {

// N represents the number of bins that the Hashes are separated into
template <SizeT N>
class StringPool {
public:
   StringPool( String filename ) : str_array( filename ) {}

   // Will find the offset (if this string was seen
   SizeT get_offset( String str );

   char* access_offset( SizeT offset );
private:
   SavedStrings str_array;
   Pair<OffsetLookupChunk<String, SizeT, Hash<String>>, Mutex> offset_hashes[N];
   Hash<String> hasher;
};

}; //namespace fb
