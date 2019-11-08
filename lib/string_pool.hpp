// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "stddef.hpp"
#include "SavedStrings.hpp"
#include "unordered_set.hpp"
#include "offset_lookup.hpp"
#include "mutex.hpp"
#include "utilities.hpp" // for pair
#include <string> // TODO switch it out
#define String std::string

namespace fb {

// N represents the number of bins that the Hashes are separated into
template <SizeT N>
class StringPool {
public:
   StringPool( String filename ) : str_array( filename ) {}

   // Will find the offset (if this string was seen
   SizeT get_offset( String str ) {
      SizeT hash = hasher(str);
      AutoLock<Mutex> l(offset_hashes[hash % N].second);
      offset_hashes[hash % N].first.find( str, hash );
   }

   char* access_offset( SizeT offset ) {
      return str_array.get_str(offset);
   }
private:
   SavedStrings str_array;
   Pair<OffsetLookupChunk<String, SizeT, Hash<String>>, Mutex> offset_hashes[N];
   Hash<String> hasher;
};

}; //namespace fb
