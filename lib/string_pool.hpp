// Created by Jaeyoon Kim 11/7/19
#pragma once

#include "stddef.hpp"
#include "saved_urls.hpp"
#include "unordered_set.hpp"
#include "url_pool.hpp"
#include "offset_lookup.hpp"
#include "mutex.hpp"
//#include "string,hpp"
//#include "vector.hpp"
#include <string>
#include <vector>

namespace fb {

// N represents the number of bins that the Hashes are separated into
template <SizeT N>
class StringPool {
public:
   StringPool( std::string filename ) : str_array( filename ) {}
   
   // Will find the offset (if this string was seen
   SizeT get_offset( std::string str );

   char* access_offset( SizeT offset );
private:
   SavedStrings str_array;
   Pair<offset_lookup, Mutex> offset_hashes[N];
};

}; //namespace fb
